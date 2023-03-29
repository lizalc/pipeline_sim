// Donavan Lance (dlance)
// ECE 463 Fall 2018

#include "pipeline_register.h"

RegisterBase::RegisterBase(unsigned long width) : width{width}
{}

InOrderRegister::InOrderRegister(unsigned long width) : RegisterBase{width}
{}

void InOrderRegister::add(Instruction *instruction)
{
	instructions.push(instruction);
}

Instruction *InOrderRegister::pop()
{
	auto ret = instructions.front();
	instructions.pop();
	return ret;
}

bool InOrderRegister::ready()
{
	return instructions.size() < width;
}

OutOfOrderRegister::OutOfOrderRegister(unsigned long width) : RegisterBase{width}
{}

void OutOfOrderRegister::add(Instruction *instruction)
{
	instructions.emplace_back(instruction);
}

Instruction *OutOfOrderRegister::pop()
{
	// Will need erase-remove idiom and to figure out how to actually work
	// this.
	return instructions.front();
}

bool OutOfOrderRegister::ready()
{
	return instructions.size() < width;
}
