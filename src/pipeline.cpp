// Donavan Lance (dlance)
// ECE 463 Fall 2018

#include "pipeline.h"
#include <iostream>
#include <vector>

Pipeline::Pipeline(unsigned long robSize, unsigned long IQSize, unsigned long width)
    : width{width}, overallCycle{0}, currentIndex{0}
{
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

				registers[PipelineRegister::ROB]->add(instruction);
				registers[PipelineRegister::RR]->add(instruction);
			}
		}
	}
}

void Pipeline::registerRead()
{
	if (!registers[PipelineRegister::RR]->empty()) {
		// Need to handle readiness of instructions here
		// as this is where things can get out of order.
		// Essentially, need to handle dependencies in the
		// instructions. Something about wakeup too.
		// FIXME -> Ignoring for now
		if (registers[PipelineRegister::DI]->ready()) {
			while (!registers[PipelineRegister::RR]->empty()) {
				auto instruction = registers[PipelineRegister::RR]->pop();
				instruction->initCycle(PipelineStage::RegisterRead, overallCycle);
				instruction->updateCycle(PipelineStage::RegisterRead);
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

		// FIXME -> Issue UP TO width instructions that are READY
		while (!registers[PipelineRegister::IQ])
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
	return overallCycle < 5;
}

void Pipeline::printInstructions() const
{
	for (const auto &i : instructionCache) {
		std::cout << *i;
	}
}
