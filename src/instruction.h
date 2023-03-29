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
	void initCycle(PipelineStage stage, int cycle);
	// Increments current stage cycle count.
	void updateCycle(PipelineStage stage);

	void dest(int index);
	void src1(int index);
	void src2(int index);
	int destOrig() const;
	int src1Orig() const;
	int src2Orig() const;
	int dest() const;
	int src1() const;
	int src2() const;

	int op() const;
	void execute();
	int executeCount() const;

	void markSrc1Ready();
	void markSrc2Ready();
	void markComplete();
	bool isReady() const;
	bool isComplete() const;

private:
	const unsigned long pc;
	InstructionMetadata data;
	int destRobIndex, src1RobIndex, src2RobIndex;
	// Number of execution cycles instruction has performed.
	int executionCount;
	bool complete;
	bool src1Ready, src2Ready;

	friend std::ostream &operator<<(std::ostream &stream, const Instruction &instr);
};

#endif  // SIM_INSTRUCTION_H
