// Donavan Lance (dlance)
// ECE 463 Fall 2018

#include "pipeline_register.h"
#include <algorithm>
#include <iostream>

RegisterBase::RegisterBase(int width) : width{width}
{}

InOrderRegister::InOrderRegister(int width) : RegisterBase{width}
{}

int InOrderRegister::add(std::shared_ptr<Instruction> instruction)
{
	instructions.push_back(instruction);
	return static_cast<int>(instructions.size()) - 1;
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

void InOrderRegister::remove(std::shared_ptr<Instruction> instruction)
{
	instructions.erase(
	    std::remove(instructions.begin(), instructions.end(), instruction),
	    instructions.end());
}

void InOrderRegister::remove(int)
{
	std::cout << "Wrong remove called\n";
	std::exit(EXIT_FAILURE);
}

bool InOrderRegister::ready() const
{
	return static_cast<int>(instructions.size()) < width;
}

bool InOrderRegister::empty() const
{
	return instructions.empty();
}

int InOrderRegister::tailIndex() const
{
	return static_cast<int>(instructions.size() - 1);
}

ReorderBuffer::ReorderBuffer(int robSize, int pipelineWidth)
    : InOrderRegister{robSize}, pipelineWidth{pipelineWidth}, head{0}
{}

void ReorderBuffer::remove(int)
{
	++head;
}

bool ReorderBuffer::ready() const
{
	// Check that there is enough space left in ROB for incoming instructions.
	return (tailIndex() - head) < width;
}

bool ReorderBuffer::empty() const
{
	return head == tailIndex();
}

IssueQueue::IssueQueue(int IQSize, int width)
    : InOrderRegister{IQSize}, pipelineWidth{width}
{}

bool IssueQueue::ready() const
{
	return pipelineWidth < (width - static_cast<int>(instructions.size()));
}
