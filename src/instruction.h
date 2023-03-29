// Donavan Lance (dlance)
// ECE 463 Fall 2018
// Project 3 - Superscalar Pipeline Simulator

#ifndef SIM_INSTRUCTION_H
#define SIM_INSTRUCTION_H

#include "instruction_metadata.h"

class Instruction {
public:
	Instruction(int sequenceNum, int opType, int destReg, int srcReg1, int srcReg2);

private:
	InstructionMetadata data;

	friend std::ostream &operator<<(std::ostream &stream, const Instruction &instr);
};

#endif  // SIM_INSTRUCTION_H
