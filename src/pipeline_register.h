// Donavan Lance (dlance)
// ECE 463 Fall 2018

#ifndef SIM_PIPELINE_REGISTER_H
#define SIM_PIPELINE_REGISTER_H

#include "instruction.h"
#include <memory>
#include <queue>
#include <vector>

// Registers can be in-order or out-of-order (execute_list / writeback out-of-order
// with rest in-order. Provide a common interface for them.
// XXX -> May need adjustments since they may need to "pop" multiple instructions
//        at once
class RegisterBase {
public:
	explicit RegisterBase(unsigned long width);
	virtual ~RegisterBase() = default;

	virtual void add(std::shared_ptr<Instruction> instruction) = 0;
	virtual std::shared_ptr<Instruction> pop() = 0;
	virtual bool ready() const = 0;
	virtual bool empty() const = 0;

protected:
	const unsigned long width;
};

class InOrderRegister : public RegisterBase {
public:
	explicit InOrderRegister(unsigned long width);

	void add(std::shared_ptr<Instruction> instruction) override;
	std::shared_ptr<Instruction> pop() override;
	bool ready() const override;
	bool empty() const override;

protected:
	std::queue<std::shared_ptr<Instruction>> instructions;
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

class OutOfOrderRegister : public RegisterBase {
public:
	explicit OutOfOrderRegister(unsigned long width);

	void add(std::shared_ptr<Instruction> instruction) override;
	std::shared_ptr<Instruction> pop() override;
	bool ready() const override;
	bool empty() const override;

private:
	std::vector<std::shared_ptr<Instruction>> instructions;
};

#endif  // SIM_PIPELINE_REGISTER_H
