// Donavan Lance (dlance)
// ECE 463 Fall 2018
// Project 3 - Superscalar Pipeline Simulator

#include "instruction.h"
#include <ostream>

Instruction::Instruction(unsigned long pc, int sequenceNum, int opType, int destReg,
                         int srcReg1, int srcReg2)
    : pc{pc},
      data{sequenceNum, opType, destReg, srcReg1, srcReg2},
      destRobIndex{-1},
      src1RobIndex{-1},
      src2RobIndex{-1},
      executionCount{0},
      complete{false},
      src1Ready{false},
      src2Ready{false}
{}

void Instruction::initCycle(PipelineStage stage, unsigned long cycle)
{
	data.initCycle(stage, cycle);
}

void Instruction::updateCycle(PipelineStage stage)
{
	data.updateCycle(stage);
}

void Instruction::dest(int index)
{
	destRobIndex = index;
}

void Instruction::src1(int index)
{
	src1RobIndex = index;
}

void Instruction::src2(int index)
{
	src2RobIndex = index;
}

int Instruction::destOrig() const
{
	return data.dest();
}

int Instruction::src1Orig() const
{
	return data.src1();
}

int Instruction::src2Orig() const
{
	return data.src2();
}

int Instruction::dest() const
{
	return destRobIndex;
}

int Instruction::src1() const
{
	return src1RobIndex;
}

int Instruction::src2() const
{
	return src2RobIndex;
}

void Instruction::execute()
{
	++executionCount;
}

int Instruction::executeCount() const
{
	return executionCount;
}

void Instruction::markSrc1Ready()
{
	src1Ready = true;
}

void Instruction::markSrc2Ready()
{
	src2Ready = true;
}

void Instruction::markComplete()
{
	complete = true;
}

bool Instruction::isReady() const
{
	return src1Ready && src2Ready;
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
