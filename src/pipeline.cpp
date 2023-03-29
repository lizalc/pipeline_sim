// Donavan Lance (dlance)
// ECE 463 Fall 2018

#include "pipeline.h"
#include <iostream>
#include <vector>

Pipeline::Pipeline(int robSize, int IQSize, int width)
    : width{width}, overallCycle{0}, currentIndex{0}
{
	for (int i = 0; i < 67; ++i) {
		renameMapTable[i] = -1;
	}

	registers[PipelineRegister::DE] =
	    std::unique_ptr<RegisterBase>(new InOrderRegister(width));

	registers[PipelineRegister::RN] =
	    std::unique_ptr<RegisterBase>(new InOrderRegister(width));

	registers[PipelineRegister::RR] =
	    std::unique_ptr<RegisterBase>(new InOrderRegister(width));

	registers[PipelineRegister::DI] =
	    std::unique_ptr<RegisterBase>(new InOrderRegister(width));

	registers[PipelineRegister::IQ] =
	    std::unique_ptr<RegisterBase>(new IssueQueue(IQSize, width));

	registers[PipelineRegister::execute_list] =
	    std::unique_ptr<RegisterBase>(new InOrderRegister(width * 5));

	registers[PipelineRegister::WB] =
	    std::unique_ptr<RegisterBase>(new InOrderRegister(width * 5));

	registers[PipelineRegister::ROB] =
	    std::unique_ptr<RegisterBase>(new ReorderBuffer(robSize, width));
}

void Pipeline::fetch()
{
	if ((currentIndex < instructionCache.size()) &&
	    (registers[PipelineRegister::DE]->ready())) {
		// Fetch <width> or fewer instructions
		for (int i = 0; (i < width) && (currentIndex < instructionCache.size());
		     i++) {
			auto instruction = instructionCache[currentIndex];
			++currentIndex;

			instruction->initCycle(PipelineStage::Fetch, overallCycle);
			instruction->updateCycle(PipelineStage::Fetch);
			registers[PipelineRegister::DE]->add(instruction);
		}
	}
}

void Pipeline::decode()
{
	if ((!registers[PipelineRegister::DE]->empty()) &&
	    (registers[PipelineRegister::RN]->ready())) {
		while (!registers[PipelineRegister::DE]->empty()) {
			auto instruction = registers[PipelineRegister::DE]->pop();
			instruction->initCycle(PipelineStage::Decode, overallCycle);
			instruction->updateCycle(PipelineStage::Decode);
			registers[PipelineRegister::RN]->add(instruction);
		}
	}
}

void Pipeline::rename()
{
	if (!registers[PipelineRegister::RN]->empty()) {
		if (registers[PipelineRegister::RR]->ready() &&
		    registers[PipelineRegister::ROB]->ready()) {
			// Rename registers
			// TODO -> Check if instructions need to actually be renamed
			//         or just allocated into the ROB. For now just allocate
			while (!registers[PipelineRegister::RN]->empty()) {
				auto instruction = registers[PipelineRegister::RN]->pop();
				instruction->initCycle(PipelineStage::Rename, overallCycle);
				instruction->updateCycle(PipelineStage::Rename);

				if (instruction->src1Orig() != -1) {
					// Rename to current ROB if need be otherwise read is from ARF
					// or immediate value
					if (renameMapTable[instruction->src1Orig()] != -1) {
						instruction->src1(renameMapTable[instruction->src1Orig()]);
					}
				}

				if (instruction->src2Orig() != -1) {
					// Rename to current ROB if need be otherwise read is from ARF
					// or immediate value
					if (renameMapTable[instruction->src2Orig()] != -1) {
						instruction->src2(renameMapTable[instruction->src2Orig()]);
					}
				}

				// Allocate in ROB and update RMT / Instruction
				registers[PipelineRegister::ROB]->add(instruction);
				instruction->dest(registers[PipelineRegister::ROB]->tailIndex());
				renameMapTable[instruction->destOrig()] = instruction->dest();

				registers[PipelineRegister::RR]->add(instruction);
			}
		}
	}
}

void Pipeline::registerRead()
{
	if (!registers[PipelineRegister::RR]->empty()) {
		if (registers[PipelineRegister::DI]->ready()) {
			while (!registers[PipelineRegister::RR]->empty()) {
				auto instruction = registers[PipelineRegister::RR]->pop();
				instruction->initCycle(PipelineStage::RegisterRead, overallCycle);
				instruction->updateCycle(PipelineStage::RegisterRead);

				if (instruction->src1() == -1) {
					instruction->markSrc1Ready();
				} else {
					// Check if instruction ROB points to is complete (writeback
					// done).
					// XXX -> May have problems here depending on when instructions
					//        are retired and cleared from ROB
					if (registers[PipelineRegister::ROB]
					        ->at(instruction->src1())
					        ->isComplete()) {
						instruction->markSrc1Ready();
					}
				}

				if (instruction->src2() == -1) {
					instruction->markSrc2Ready();
				} else {
					// Check if instruction ROB points to is complete (writeback
					// done).
					// XXX -> May have problems here depending on when instructions
					//        are retired and cleared from ROB
					if (registers[PipelineRegister::ROB]
					        ->at(instruction->src2())
					        ->isComplete()) {
						instruction->markSrc2Ready();
					}
				}

				registers[PipelineRegister::DI]->add(instruction);
			}
		}
	}
}

void Pipeline::dispatch()
{
	if (!registers[PipelineRegister::DI]->empty() &&
	    registers[PipelineRegister::IQ]->ready()) {
		while (!registers[PipelineRegister::DI]->empty()) {
			auto instruction = registers[PipelineRegister::DI]->pop();
			instruction->initCycle(PipelineStage::Dispatch, overallCycle);
			instruction->updateCycle(PipelineStage::Dispatch);

			if (instruction->src1() == -1) {
				instruction->markSrc1Ready();
			} else {
				// Check if instruction ROB points to is complete (writeback
				// done).
				// XXX -> May have problems here depending on when instructions
				//        are retired and cleared from ROB
				if (registers[PipelineRegister::ROB]
				        ->at(instruction->src1())
				        ->isComplete()) {
					instruction->markSrc1Ready();
				}
			}

			if (instruction->src2() == -1) {
				instruction->markSrc2Ready();
			} else {
				// Check if instruction ROB points to is complete (writeback
				// done).
				// XXX -> May have problems here depending on when instructions
				//        are retired and cleared from ROB
				if (registers[PipelineRegister::ROB]
				        ->at(instruction->src2())
				        ->isComplete()) {
					instruction->markSrc2Ready();
				}
			}

			registers[PipelineRegister::IQ]->add(instruction);
		}
	}
}

void Pipeline::issue()
{
	if (!registers[PipelineRegister::IQ]->empty() &&
	    // FIXME -> Execute_list ready is more complex than
	    //          just having space available.
	    registers[PipelineRegister::execute_list]->ready()) {
		// Keep a temporary vector of issued instructions so they can be properly
		// removed from issue queue.
		std::vector<std::shared_ptr<Instruction>> readyInstructions;

		// XXX -> May need to do more than check if the execute_list is full
		for (int i = 0, readyCount = 0;
		     (i <= registers[PipelineRegister::IQ]->tailIndex()) &&
		     (readyCount < width) &&
		     registers[PipelineRegister::execute_list]->ready();
		     ++i) {
			// Iterating in order, so oldest instruction is first, youngest last.
			// If instruction is ready, it is issued, until either no more ready
			// instructions remain, max width has been reached, or the execute_list
			// is full.
			auto instruction = registers[PipelineRegister::IQ]->at(i);
			// XXX -> Double check that this counts correctly for stalled / sent
			//        instructions
			instruction->initCycle(PipelineStage::Issue, overallCycle);
			instruction->updateCycle(PipelineStage::Issue);

			if (instruction->src1() == -1) {
				instruction->markSrc1Ready();
			} else {
				// Check if instruction ROB points to is complete (writeback
				// done).
				// XXX -> May have problems here depending on when instructions
				//        are retired and cleared from ROB
				if (registers[PipelineRegister::ROB]
				        ->at(instruction->src1())
				        ->isComplete()) {
					instruction->markSrc1Ready();
				}
			}

			if (instruction->src2() == -1) {
				instruction->markSrc2Ready();
			} else {
				// Check if instruction ROB points to is complete (writeback
				// done).
				// XXX -> May have problems here depending on when instructions
				//        are retired and cleared from ROB
				if (registers[PipelineRegister::ROB]
				        ->at(instruction->src2())
				        ->isComplete()) {
					instruction->markSrc2Ready();
				}
			}

			if (instruction->isReady()) {
				++readyCount;
				readyInstructions.push_back(instruction);
				registers[PipelineRegister::execute_list]->add(instruction);
			}
		}

		for (auto &i : readyInstructions) {
			registers[PipelineRegister::IQ]->remove(i);
		}
	}
}

void Pipeline::execute()
{
	if (!registers[PipelineRegister::execute_list]->empty()) {
		// XXX -> Double check correct execution cycles (increment execution
		//        counters then check for finish, or check for finish then
		//        increment? Instructions make it seem like the former, just be sure)
		// Keep a vector of completed instructions for removal at the end
		std::vector<std::shared_ptr<Instruction>> completeInstructions;

		for (int i = 0; i <= registers[PipelineRegister::execute_list]->tailIndex();
		     ++i) {
			auto instruction = registers[PipelineRegister::execute_list]->at(i);
			instruction->initCycle(PipelineStage::Execute, overallCycle);
			instruction->updateCycle(PipelineStage::Execute);
			instruction->execute();

			switch (instruction->op()) {
			case 0:
				if (instruction->executeCount() == 1) {
					instruction->markExecuteDone();
				}
				break;

			case 1:
				if (instruction->executeCount() == 2) {
					instruction->markExecuteDone();
				}
				break;

			case 2:
				if (instruction->executeCount() == 5) {
					instruction->markExecuteDone();
				}
				break;

			default:
				break;
			}

			if (instruction->executeDone()) {
				// XXX -> May have too many instructions completing at once with
				//        the way code is now. Not 100% sure if so or how to handle.
				completeInstructions.push_back(instruction);
				registers[PipelineRegister::WB]->add(instruction);
			}
		}

		for (auto &i : completeInstructions) {
			registers[PipelineRegister::execute_list]->remove(i);
		}
	}
}

void Pipeline::writeback()
{
	if (!registers[PipelineRegister::WB]->empty()) {
		// ROB already has instructions marked as ready from the execute stage
		// Clear the writeback register
		while (!registers[PipelineRegister::WB]->empty()) {
			auto instruction = registers[PipelineRegister::WB]->pop();
			instruction->initCycle(PipelineStage::Writeback, overallCycle);
			instruction->updateCycle(PipelineStage::Writeback);
			instruction->markComplete();
			instruction->markRetire();
		}
	}
}

void Pipeline::retire()
{
	if (!registers[PipelineRegister::ROB]->empty()) {
		// Keep a vector of completed instructions for removal at the end
		std::vector<int> completeInstructions;

		for (int i = registers[PipelineRegister::ROB]->headIndex();
		     i <= registers[PipelineRegister::ROB]->tailIndex(); ++i) {
			auto instruction = registers[PipelineRegister::ROB]->at(i);

			if (instruction->isRetired()) {
				// XXX -> Right check against width?
				if (static_cast<int>(completeInstructions.size()) < width) {
					instruction->initCycle(PipelineStage::Retire, overallCycle);
					instruction->updateCycle(PipelineStage::Retire);
					completeInstructions.push_back(i);

					// Check and reset RMT if needed
					if (registers[PipelineRegister::ROB]->at(
					        renameMapTable[instruction->destOrig()]) ==
					    instruction) {
						renameMapTable[instruction->destOrig()] = -1;
					}

				} else {
					break;
				}
			}
		}

		for (auto &i : completeInstructions) {
			registers[PipelineRegister::ROB]->remove(i);
		}
	}
}

void Pipeline::advanceCycle()
{
	++overallCycle;
}

void Pipeline::addToInstructionCache(std::shared_ptr<Instruction> instruction)
{
	instructionCache.emplace_back(instruction);
}

bool Pipeline::instructionsFinished() const
{
	// return currentIndex < instructionCache.size();
	return overallCycle < 80;
}

void Pipeline::printInstructions() const
{
	for (const auto &i : instructionCache) {
		std::cout << *i;
	}
}
