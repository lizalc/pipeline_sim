// Donavan Lance (dlance)
// ECE 463 Fall 2018
// Project 3 - Superscalar Pipeline Simulator

#include "instruction.h"
#include <ostream>

Instruction::Instruction(unsigned long pc, int sequenceNum, int opType, int destReg,
                         int srcReg1, int srcReg2)
    : pc{pc}, data{sequenceNum, opType, destReg, srcReg1, srcReg2}
{}

std::ostream &operator<<(std::ostream &stream, const Instruction &instr)
{
	// Don't force the metadata to handle formatting (newlines)
	return stream << instr.data << '\n';
}
