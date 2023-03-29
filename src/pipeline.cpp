// Donavan Lance (dlance)
// ECE 463 Fall 2018

#include "pipeline.h"
#include <iostream>

Pipeline::Pipeline()
{}

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
