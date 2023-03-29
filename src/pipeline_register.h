// Donavan Lance (dlance)
// ECE 463 Fall 2018

#ifndef SIM_PIPELINE_REGISTER_H
#define SIM_PIPELINE_REGISTER_H

#include "instruction.h"
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

	virtual void add(Instruction *instruction) = 0;
	virtual Instruction *pop() = 0;
	virtual bool ready() = 0;

protected:
	const unsigned long width;
};

class InOrderRegister : public RegisterBase {
public:
	explicit InOrderRegister(unsigned long width);

	void add(Instruction *instruction) override;
	Instruction *pop() override;
	bool ready() override;

private:
	std::queue<Instruction *> instructions;
};

class OutOfOrderRegister : public RegisterBase {
public:
	explicit OutOfOrderRegister(unsigned long width);

	void add(Instruction *instruction) override;
	Instruction *pop() override;
	bool ready() override;

private:
	std::vector<Instruction *> instructions;
};

#endif  // SIM_PIPELINE_REGISTER_H
