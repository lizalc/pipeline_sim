// Donavan Lance (dlance)
// ECE 463 Fall 2018

#include "pipeline.h"
#include <iostream>
#include <unordered_set>
#include <vector>

Pipeline::Pipeline(int robSize, int IQSize, int width)
    : reorderBuffer{robSize, width},
      width{width},
      overallCycle{0},
      currentIndex{0},
      lastInstructionRetired{false}
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
}

void Pipeline::fetch()
{
	if ((currentIndex < instructions.size()) &&
	    (registers[PipelineRegister::DE]->ready())) {
		// Fetch <width> or fewer instructions
		for (int i = 0; (i < width) && (currentIndex < instructions.size()); i++) {
			auto instruction = instructions[currentIndex];
			++currentIndex;

			instruction->initCycle(PipelineStage::Fetch, overallCycle);
			instruction->updateCycle(PipelineStage::Fetch);
			registers[PipelineRegister::DE]->add(instruction);
		}
	}
}

void Pipeline::decode()
{
	if (!registers[PipelineRegister::DE]->empty()) {
		for (int i = 0; i <= registers[PipelineRegister::DE]->tailIndex(); ++i) {
			auto instruction = registers[PipelineRegister::DE]->at(i);
			instruction->initCycle(PipelineStage::Decode, overallCycle);
			instruction->updateCycle(PipelineStage::Decode);
		}

		if (registers[PipelineRegister::RN]->ready()) {
			while (!registers[PipelineRegister::DE]->empty()) {
				auto instruction = registers[PipelineRegister::DE]->pop();
				registers[PipelineRegister::RN]->add(instruction);
			}
		}
	}
}

void Pipeline::rename()
{
	if (!registers[PipelineRegister::RN]->empty()) {
		// Increment all pipeline cycle counters
		for (int i = 0; i <= registers[PipelineRegister::RN]->tailIndex(); ++i) {
			auto instruction = registers[PipelineRegister::RN]->at(i);
			instruction->initCycle(PipelineStage::Rename, overallCycle);
			instruction->updateCycle(PipelineStage::Rename);
		}

		if (registers[PipelineRegister::RR]->ready() && reorderBuffer.ready()) {
			// Rename registers
			// TODO -> Check if instructions need to actually be renamed
			//         or just allocated into the ROB. For now just allocate
			while (!registers[PipelineRegister::RN]->empty()) {
				auto instruction = registers[PipelineRegister::RN]->pop();

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

				// Allocate in ROB
				int renamedDest = reorderBuffer.allocate(
				    instruction->destOrig(), instruction->pc(), instruction);
				// Rename destination
				instruction->dest(renamedDest);
				// Update RMT if needed
				if (instruction->destOrig() != -1) {
					renameMapTable[instruction->destOrig()] = renamedDest;
				}

				registers[PipelineRegister::RR]->add(instruction);
			}
		}
	}
}

void Pipeline::registerRead()
{
	if (!registers[PipelineRegister::RR]->empty()) {
		for (int i = 0; i <= registers[PipelineRegister::RR]->tailIndex(); ++i) {
			auto instruction = registers[PipelineRegister::RR]->at(i);
			instruction->initCycle(PipelineStage::RegisterRead, overallCycle);
			instruction->updateCycle(PipelineStage::RegisterRead);

			if (instruction->src1() == -1) {
				instruction->markSrc1Ready();
			} else {
				// Check if instruction ROB points to is complete (writeback
				// done).
				if (reorderBuffer.valueReady(instruction->src1()) ||
				    reorderBuffer.executeValueReady(instruction->src1())) {
					instruction->markSrc1Ready();
				}
			}

			if (instruction->src2() == -1) {
				instruction->markSrc2Ready();
			} else {
				// Check if instruction ROB points to is complete (writeback
				// done).
				if (reorderBuffer.valueReady(instruction->src2()) ||
				    reorderBuffer.executeValueReady(instruction->src2())) {
					instruction->markSrc2Ready();
				}
			}
		}

		if (registers[PipelineRegister::DI]->ready()) {
			while (!registers[PipelineRegister::RR]->empty()) {
				auto instruction = registers[PipelineRegister::RR]->pop();
				registers[PipelineRegister::DI]->add(instruction);
			}
		}
	}
}

void Pipeline::dispatch()
{
	if (!registers[PipelineRegister::DI]->empty()) {
		for (int i = 0; i <= registers[PipelineRegister::DI]->tailIndex(); ++i) {
			auto instruction = registers[PipelineRegister::DI]->at(i);
			instruction->initCycle(PipelineStage::Dispatch, overallCycle);
			instruction->updateCycle(PipelineStage::Dispatch);

			if (instruction->src1() == -1) {
				instruction->markSrc1Ready();
			} else {
				// Check if instruction ROB points to is complete (writeback
				// done).
				if (reorderBuffer.valueReady(instruction->src1()) ||
				    reorderBuffer.executeValueReady(instruction->src1())) {
					instruction->markSrc1Ready();
				}
			}

			if (instruction->src2() == -1) {
				instruction->markSrc2Ready();
			} else {
				// Check if instruction ROB points to is complete (writeback
				// done).
				if (reorderBuffer.valueReady(instruction->src2()) ||
				    reorderBuffer.executeValueReady(instruction->src2())) {
					instruction->markSrc2Ready();
				}
			}
		}

		if (registers[PipelineRegister::IQ]->ready()) {
			while (!registers[PipelineRegister::DI]->empty()) {
				auto instruction = registers[PipelineRegister::DI]->pop();
				registers[PipelineRegister::IQ]->add(instruction);
			}
		}
	}
}

void Pipeline::issue()
{
	if (!registers[PipelineRegister::IQ]->empty()) {
		for (int i = 0; (i <= registers[PipelineRegister::IQ]->tailIndex()); ++i) {
			auto instruction = registers[PipelineRegister::IQ]->at(i);
			instruction->initCycle(PipelineStage::Issue, overallCycle);
			instruction->updateCycle(PipelineStage::Issue);

			if (instruction->src1() == -1) {
				instruction->markSrc1Ready();
			} else {
				// Check if instruction ROB points to is complete (writeback
				// done).
				if (reorderBuffer.valueReady(instruction->src1()) ||
				    reorderBuffer.executeValueReady(instruction->src1())) {
					instruction->markSrc1Ready();
				}
			}

			if (instruction->src2() == -1) {
				instruction->markSrc2Ready();
			} else {
				// Check if instruction ROB points to is complete (writeback
				// done).
				if (reorderBuffer.valueReady(instruction->src2()) ||
				    reorderBuffer.executeValueReady(instruction->src2())) {
					instruction->markSrc2Ready();
				}
			}
		}

		if (registers[PipelineRegister::execute_list]->ready()) {
			int readyCount = 0;
			for (int i = 0; (i <= registers[PipelineRegister::IQ]->tailIndex()) &&
			                (readyCount < width) &&
			                registers[PipelineRegister::execute_list]->ready();
			     ++i) {
				// Iterating in order, so oldest instruction is first, youngest last.
				// If instruction is ready, it is issued, until either no more ready
				// instructions remain, max width has been reached, or the
				// execute_list is full.
				auto instruction = registers[PipelineRegister::IQ]->at(i);

				if (instruction->isReady()) {
					++readyCount;
					registers[PipelineRegister::execute_list]->add(instruction);
					registers[PipelineRegister::IQ]->remove(instruction);
					// Popped current i, queue moves everything down causing
					// i to need to remain the same for next iteration.
					--i;
				}
			}
		}
	}
}

void Pipeline::execute()
{
	if (!registers[PipelineRegister::execute_list]->empty()) {
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
				completeInstructions.push_back(instruction);
				registers[PipelineRegister::WB]->add(instruction);
				reorderBuffer.markExecuteValueReady(instruction->dest());
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
			reorderBuffer.markValueReady(instruction->dest());
		}
	}
}

void Pipeline::retire()
{
	if (!reorderBuffer.empty()) {
		int count = 0;

		// Find all retiring instructions and increment counter
		for (int i = 0; i < reorderBuffer.totalSize(); ++i) {
			if (!reorderBuffer.fullyRetired(i)) {
				auto instruction = reorderBuffer.at(i);
				if ((instruction != nullptr) && instruction->isComplete()) {
					instruction->initCycle(PipelineStage::Retire, overallCycle);
					instruction->updateCycle(PipelineStage::Retire);
				}
			}
		}

		while (count < width) {
			++count;
			auto data = reorderBuffer.peek();

			// See if ROB head can be retired
			if (data.ready) {
				// Pop the head
				int robIndex = reorderBuffer.headIndex();
				reorderBuffer.pop();
				if (data.destReg != -1) {
					if (renameMapTable[data.destReg] == robIndex) {
						renameMapTable[data.destReg] = -1;
					}
				}
				data.instruction->markFullyRetired();
			} else {
				// Retire in program order, so if head can't be popped, nothing
				// will be popped.
				break;
			}
		}
	}
}

void Pipeline::advanceCycle()
{
	++overallCycle;
}

void Pipeline::addToInstructionCache(std::shared_ptr<Instruction> instruction)
{
	instructions.push_back(instruction);
}

bool Pipeline::instructionsFinished() const
{
	if (currentIndex < instructions.size()) {
		return true;
	} else {
		bool ret = false;

		for (int i = 0; i < reorderBuffer.totalSize(); ++i) {
			if (!reorderBuffer.fullyRetired(i)) {
				ret = true;
			}
		}

		return ret;
	}
}

void Pipeline::markLastInstruction()
{
	instructions.back()->markFinalInstruction();
}

void Pipeline::printInstructions() const
{
	for (const auto &i : instructions) {
		std::cout << *i;
	}
}

int Pipeline::getTotalCycles() const
{
	return overallCycle;
}

int Pipeline::getInstructionCount() const
{
	return static_cast<int>(instructions.size());
}
