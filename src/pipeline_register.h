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
	virtual void remove(int index) = 0;
	virtual bool ready() const = 0;
	virtual bool empty() const = 0;

	virtual int tailIndex() const = 0;
	int getWidth() const
	{
		return static_cast<int>(width);
	}

	// For the ROB
	virtual int headIndex() const
	{
		return -1;
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
	void remove(int) override;
	bool ready() const override;
	bool empty() const override;
	int tailIndex() const override;

protected:
	std::deque<std::shared_ptr<Instruction>> instructions;
};

class ReorderBuffer : public InOrderRegister {
public:
	ReorderBuffer(int robSize, int width);
	void remove(int) override;
	bool ready() const override;
	bool empty() const override;

	int headIndex() const override
	{
		return head;
	}

private:
	int pipelineWidth;
	int head;
};

class IssueQueue : public InOrderRegister {
public:
	IssueQueue(int IQSize, int width);
	bool ready() const override;

private:
	int pipelineWidth;
};

#endif  // SIM_PIPELINE_REGISTER_H
