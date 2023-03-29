// Donavan Lance (dlance)
// ECE 463 Fall 2018

#include "pipeline.h"
#include <iostream>

Pipeline::Pipeline(unsigned long robSize, unsigned long IQSize, unsigned long width)
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
	    std::unique_ptr<RegisterBase>(new InOrderRegister(IQSize));

	registers[PipelineRegister::execute_list] =
	    std::unique_ptr<RegisterBase>(new OutOfOrderRegister(width * 5));

	registers[PipelineRegister::WB] =
	    std::unique_ptr<RegisterBase>(new OutOfOrderRegister(width * 5));

	// Should ROB be in order or out of order? Actually, this should be
	// in order with respect to program order. May need a new type for it.
	registers[PipelineRegister::ROB] =
	    std::unique_ptr<RegisterBase>(new InOrderRegister(robSize));
}

void Pipeline::fetch()
{}

void Pipeline::decode()
{}

void Pipeline::rename()
{}

void Pipeline::registerRead()
{}

void Pipeline::dispatch()
{}

void Pipeline::issue()
{}

void Pipeline::execute()
{}

void Pipeline::writeback()
{}

void Pipeline::retire()
{}

void Pipeline::setInstructionCache(std::vector<Instruction> &&instructions)
{
	instructionCache = std::move(instructions);
}

void Pipeline::printInstructions() const
{
	for (const auto &i : instructionCache) {
		std::cout << i;
	}
}
