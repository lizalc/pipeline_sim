// Donavan Lance (dlance)
// ECE 463 Fall 2018

#include "pipeline.h"
#include <iostream>
#include <vector>

Pipeline::Pipeline(unsigned long robSize, unsigned long IQSize, unsigned long width)
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
	    std::unique_ptr<RegisterBase>(new OutOfOrderRegister(width * 5));

	registers[PipelineRegister::WB] =
	    std::unique_ptr<RegisterBase>(new OutOfOrderRegister(width * 5));

	registers[PipelineRegister::ROB] =
	    std::unique_ptr<RegisterBase>(new ReorderBuffer(robSize, width));
}

void Pipeline::fetch()
{
	if ((currentIndex < instructionCache.size()) &&
	    (registers[PipelineRegister::DE]->ready())) {
		// Fetch <width> or fewer instructions
		for (unsigned long i = 0;
		     (i < width) && (currentIndex < instructionCache.size()); i++) {
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

				if (instruction->destOrig() != -1) {
					// Allocate in ROB and update RMT / Instruction
					registers[PipelineRegister::ROB]->add(instruction);
					instruction->dest(registers[PipelineRegister::ROB]->tailIndex());
					renameMapTable[instruction->destOrig()] = instruction->dest();
				}

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
		     (readyCount < registers[PipelineRegister::execute_list]->getWidth()) &&
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
{}

void Pipeline::writeback()
{}

void Pipeline::retire()
{}

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
	return overallCycle < 15;
}

void Pipeline::printInstructions() const
{
	for (const auto &i : instructionCache) {
		std::cout << *i;
	}
}
