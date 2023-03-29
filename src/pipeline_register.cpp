// Donavan Lance (dlance)
// ECE 463 Fall 2018

#include "pipeline_register.h"

RegisterBase::RegisterBase(unsigned long width) : width{width}
{}

InOrderRegister::InOrderRegister(unsigned long width) : RegisterBase{width}
{}

unsigned long InOrderRegister::add(std::shared_ptr<Instruction> instruction)
{
	instructions.push_back(instruction);
	return instructions.size() - 1;
}

std::shared_ptr<Instruction> InOrderRegister::pop()
{
	auto ret = instructions.front();
	instructions.pop_front();
	return ret;
}

std::shared_ptr<Instruction> InOrderRegister::at(int index)
{
	return instructions.at(static_cast<size_t>(index));
}

bool InOrderRegister::ready() const
{
	return instructions.size() < width;
}

bool InOrderRegister::empty() const
{
	return instructions.empty();
}

int InOrderRegister::tailIndex() const
{
	return static_cast<int>(instructions.size() - 1);
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

unsigned long OutOfOrderRegister::add(std::shared_ptr<Instruction> instruction)
{
	instructions.emplace_back(instruction);
	return instructions.size() - 1;
}

std::shared_ptr<Instruction> OutOfOrderRegister::pop()
{
	// Will need erase-remove idiom and to figure out how to actually work
	// this.
	return instructions.front();
}

std::shared_ptr<Instruction> OutOfOrderRegister::at(int index)
{
	return instructions.at(static_cast<size_t>(index));
}

bool OutOfOrderRegister::ready() const
{
	return instructions.size() < width;
}

bool OutOfOrderRegister::empty() const
{
	return instructions.empty();
}

int OutOfOrderRegister::tailIndex() const
{
	return static_cast<int>(instructions.size() - 1);
}
