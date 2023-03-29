// Donavan Lance (dlance)
// ECE 463 Fall 2018

#include "pipeline_register.h"

RegisterBase::RegisterBase(unsigned long width) : width{width}
{}

InOrderRegister::InOrderRegister(unsigned long width) : RegisterBase{width}
{}

void InOrderRegister::add(std::shared_ptr<Instruction> instruction)
{
	instructions.push(instruction);
}

std::shared_ptr<Instruction> InOrderRegister::pop()
{
	auto ret = instructions.front();
	instructions.pop();
	return ret;
}

bool InOrderRegister::ready() const
{
	return instructions.size() < width;
}

bool InOrderRegister::empty() const
{
	return instructions.empty();
}

ReorderBuffer::ReorderBuffer(unsigned long robSize, unsigned long pipelineWidth)
    : InOrderRegister{robSize}, pipelineWidth{pipelineWidth}
{}

bool ReorderBuffer::ready() const
{
	// Check that there is enough space left in ROB for incoming instructions.
	return pipelineWidth < (width - instructions.size());
}

OutOfOrderRegister::OutOfOrderRegister(unsigned long width) : RegisterBase{width}
{}

void OutOfOrderRegister::add(std::shared_ptr<Instruction> instruction)
{
	instructions.emplace_back(instruction);
}

std::shared_ptr<Instruction> OutOfOrderRegister::pop()
{
	// Will need erase-remove idiom and to figure out how to actually work
	// this.
	return instructions.front();
}

bool OutOfOrderRegister::ready() const
{
	return instructions.size() < width;
}

bool OutOfOrderRegister::empty() const
{
	return instructions.empty();
}
