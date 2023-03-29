// Donavan Lance (dlance)
// ECE 463 Fall 2018
// Project 3 - Superscalar Pipeline Simulator

#include "instruction.h"
#include <ostream>

Instruction::Instruction(unsigned long pc, int sequenceNum, int opType, int destReg,
                         int srcReg1, int srcReg2)
    : pc{pc},
      data{sequenceNum, opType, destReg, srcReg1, srcReg2},
      status{false},
      complete{false}
{}

void Instruction::initCycle(PipelineStage stage, unsigned long cycle)
{
	data.initCycle(stage, cycle);
}

void Instruction::updateCycle(PipelineStage stage)
{
	data.updateCycle(stage);
}

void Instruction::rename(InstructionRegister reg, int val)
{
	data.rename(reg, val);
}

void Instruction::markReady()
{
	status = true;
}

void Instruction::markComplete()
{
	complete = true;
}

bool Instruction::isReady() const
{
	return status;
}

bool Instruction::isComplete() const
{
	return complete;
}

std::ostream &operator<<(std::ostream &stream, const Instruction &instr)
{
	// Don't force the metadata to handle formatting (newlines)
	return stream << instr.data << '\n';
}
