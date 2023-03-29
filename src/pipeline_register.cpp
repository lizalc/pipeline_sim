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
    : size{robSize},
      pipelineWidth{pipelineWidth},
      head{0},
      tail{0},
      mFull{false},
      mEmpty{true}
{
	for (int i = 0; i < robSize; ++i) {
		// Initialize all ROB entries
		contents.emplace_back(BufferContents{-1, 0, nullptr});
	}
}

int ReorderBuffer::allocate(int destReg, unsigned long pc,
                            std::shared_ptr<Instruction> instr)
{
	int ret = tail;
	contents[tail] = BufferContents{destReg, pc, instr};
	++tail;

	if (tail >= static_cast<int>(contents.size())) {
		tail = 0;
	}

	if (tail == head) {
		mFull = true;
	}

	// Any entry added means ROB is not empty
	mEmpty = false;

	// Return index of allocated ROB entry
	return ret;
}

ReorderBuffer::BufferContents ReorderBuffer::pop()
{
	contents[head].fullyRetired = true;
	auto ret = contents[head];
	if (head == tail) {
		mFull = false;
	}
	++head;

	if (head >= static_cast<int>(contents.size())) {
		head = 0;
	}

	// If head now equals tail, the last entry in the ROB was just popped
	if (head == tail) {
		mEmpty = true;
	}

	return ret;
}

ReorderBuffer::BufferContents ReorderBuffer::peek()
{
	return contents[head];
}

std::shared_ptr<Instruction> ReorderBuffer::at(int index)
{
	return contents[index].instruction;
}

void ReorderBuffer::markValueReady(int index)
{
	contents[index].ready = true;
}

void ReorderBuffer::markExecuteValueReady(int index)
{
	contents[index].executed = true;
}

bool ReorderBuffer::valueReady(int index) const
{
	return contents[index].ready;
}

bool ReorderBuffer::executeValueReady(int index) const
{
	return contents[index].executed;
}

bool ReorderBuffer::fullyRetired(int index) const
{
	return contents[index].fullyRetired;
}

bool ReorderBuffer::ready() const
{
	// Handle tail < head case (wraparound) and see if there is enough space
	// to accept <WIDTH> new contents
	if (mFull) {
		return false;
	}

	if (mEmpty) {
		return true;
	}

	if (tail < head) {
		auto tmpTail = static_cast<int>(contents.size()) + tail;
		auto numContents = tmpTail - head;
		return pipelineWidth < (size - numContents + 1);
	}

	// Check that enough free space exists
	return pipelineWidth < (size - (tail - head) + 1);
}

bool ReorderBuffer::full() const
{
	return mFull;
}

bool ReorderBuffer::empty() const
{
	return mEmpty;
}

IssueQueue::IssueQueue(int IQSize, int width)
    : InOrderRegister{IQSize}, pipelineWidth{width}
{}

bool IssueQueue::ready() const
{
	return pipelineWidth < (width - static_cast<int>(instructions.size()) + 1);
}
