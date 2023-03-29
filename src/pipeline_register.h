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
	explicit RegisterBase(unsigned long width);
	virtual ~RegisterBase() = default;

	virtual unsigned long add(std::shared_ptr<Instruction> instruction) = 0;
	virtual std::shared_ptr<Instruction> pop() = 0;
	virtual std::shared_ptr<Instruction> at(int index) = 0;
	virtual void remove(std::shared_ptr<Instruction> instruction) = 0;
	virtual bool ready() const = 0;
	virtual bool empty() const = 0;
	// Used by issue queue to see how many lines are available for instructions
	// virtual int linesAvailable() const = 0;
	virtual int tailIndex() const = 0;
	int getWidth() const
	{
		return static_cast<int>(width);
	}

protected:
	const unsigned long width;
};

class InOrderRegister : public RegisterBase {
public:
	explicit InOrderRegister(unsigned long width);

	unsigned long add(std::shared_ptr<Instruction> instruction) override;
	std::shared_ptr<Instruction> pop() override;
	std::shared_ptr<Instruction> at(int index) override;
	void remove(std::shared_ptr<Instruction> instruction) override;
	bool ready() const override;
	bool empty() const override;
	int tailIndex() const override;

protected:
	std::deque<std::shared_ptr<Instruction>> instructions;
};

class ReorderBuffer : public InOrderRegister {
public:
	explicit ReorderBuffer(unsigned long robSize, unsigned long width);
	bool ready() const override;

private:
	unsigned long pipelineWidth;
};

// Just subclass ReorderBuffer to rename it. Needs the modified ready()
// as well.
class IssueQueue : public ReorderBuffer {
public:
	explicit IssueQueue(unsigned long IQSize, unsigned long width)
	    : ReorderBuffer{IQSize, width}
	{}
};

// class ExecuteList : public InOrderRegister {
// public:
//	explicit ExecuteList(unsigned long width);
//	bool ready() const override;
//};

class OutOfOrderRegister : public RegisterBase {
public:
	explicit OutOfOrderRegister(unsigned long width);

	unsigned long add(std::shared_ptr<Instruction> instruction) override;
	std::shared_ptr<Instruction> pop() override;
	std::shared_ptr<Instruction> at(int index) override;
	void remove(std::shared_ptr<Instruction> instruction) override;
	bool ready() const override;
	bool empty() const override;
	int tailIndex() const override;

private:
	std::vector<std::shared_ptr<Instruction>> instructions;
};

#endif  // SIM_PIPELINE_REGISTER_H
