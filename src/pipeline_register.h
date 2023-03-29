// Donavan Lance (dlance)
// ECE 463 Fall 2018

#ifndef SIM_PIPELINE_REGISTER_H
#define SIM_PIPELINE_REGISTER_H

#include "instruction.h"
#include <deque>
#include <memory>
#include <vector>

// Registers can be in-order or out-of-order (execute_list / writeback out-of-order
// with rest in-order. Provide a common interface for them.
// XXX -> May need adjustments since they may need to "pop" multiple instructions
//        at once
class RegisterBase {
public:
	explicit RegisterBase(int width);
	virtual ~RegisterBase() = default;

	virtual int add(std::shared_ptr<Instruction> instruction) = 0;
	virtual std::shared_ptr<Instruction> pop() = 0;
	virtual std::shared_ptr<Instruction> at(int index) = 0;
	virtual void remove(std::shared_ptr<Instruction> instruction) = 0;
	virtual bool ready() const = 0;
	virtual bool empty() const = 0;

	virtual int tailIndex() const = 0;
	int getWidth() const
	{
		return width;
	}

protected:
	const int width;
};

class InOrderRegister : public RegisterBase {
public:
	explicit InOrderRegister(int width);

	int add(std::shared_ptr<Instruction> instruction) override;
	std::shared_ptr<Instruction> pop() override;
	std::shared_ptr<Instruction> at(int index) override;
	void remove(std::shared_ptr<Instruction> instruction) override;
	bool ready() const override;
	bool empty() const override;
	int tailIndex() const override;

protected:
	std::deque<std::shared_ptr<Instruction>> instructions;
};

class ReorderBuffer {
public:
	struct BufferContents {
		BufferContents(int d, unsigned long p,
		               const std::shared_ptr<Instruction> &instr)
		    : ready{false},
		      executed{false},
		      fullyRetired{false},
		      destReg{d},
		      pc{p},
		      instruction(instr)
		{}

		bool ready, executed, fullyRetired;
		int destReg;
		unsigned long pc;
		// Pointer back to instruction that caused this ROB entry to be allocated
		std::shared_ptr<Instruction> instruction;
	};

	ReorderBuffer(int robSize, int width);

	int allocate(int destReg, unsigned long pc, std::shared_ptr<Instruction> instr);
	BufferContents pop();
	BufferContents peek();
	std::shared_ptr<Instruction> at(int index);
	void markValueReady(int index);
	void markExecuteValueReady(int index);
	bool valueReady(int index) const;
	bool executeValueReady(int index) const;
	bool fullyRetired(int index) const;
	bool ready() const;
	bool full() const;
	bool empty() const;

	int headIndex() const
	{
		return head;
	}

	int tailIndex() const
	{
		return tail;
	}

	int totalSize() const
	{
		return size;
	}

private:
	std::vector<BufferContents> contents;
	int size, pipelineWidth;
	int head, tail;
	bool mFull, mEmpty;
};

class IssueQueue : public InOrderRegister {
public:
	IssueQueue(int IQSize, int width);
	bool ready() const override;

private:
	int pipelineWidth;
};

#endif  // SIM_PIPELINE_REGISTER_H
