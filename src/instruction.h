// Donavan Lance (dlance)
// ECE 463 Fall 2018
// Project 3 - Superscalar Pipeline Simulator

#ifndef SIM_INSTRUCTION_H
#define SIM_INSTRUCTION_H

#include "instruction_metadata.h"

class Instruction {
public:
	Instruction(unsigned long pc, int sequenceNum, int opType, int destReg,
	            int srcReg1, int srcReg2);

	// Sets the cycle a stage started
	void initCycle(PipelineStage stage, unsigned long cycle);
	// Increments current stage cycle count.
	void updateCycle(PipelineStage stage);
	void rename(InstructionRegister reg, int val);
	void markReady();
	void markComplete();
	bool isReady() const;
	bool isComplete() const;

private:
	const unsigned long pc;
	InstructionMetadata data;
	bool status;
	bool complete;

	friend std::ostream &operator<<(std::ostream &stream, const Instruction &instr);
};

#endif  // SIM_INSTRUCTION_H
