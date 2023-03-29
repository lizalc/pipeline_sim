// Donavan Lance (dlance)
// ECE 463 Fall 2018

#include "simulator.h"
#include "instruction.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

Simulator::Simulator(const int robSize, const int IQSize, const int width,
                     const std::string &traceFile)
    : robSize{robSize}, IQSize{IQSize}, width{width}, traceFile{traceFile}
{}

void Simulator::setup()
{
	std::ifstream trace(traceFile);

	if (!trace.is_open()) {
		std::cout << "Error: Unable to open file " << traceFile << '\n';
	}

	std::vector<Instruction> instructions;
	int sequenceNum = 0;
	std::string line;
	while (std::getline(trace, line)) {
		std::istringstream values{line};
		std::string discard;  // PC is discarded, not needed.
		values >> discard;

		int opType, destReg, srcReg1, srcReg2;
		values >> opType >> destReg >> srcReg1 >> srcReg2;

		instructions.emplace_back(
		    Instruction(sequenceNum, opType, destReg, srcReg1, srcReg2));
		++sequenceNum;
	}

	pipeline.setInstructionCache(std::move(instructions));
}

void Simulator::run()
{
	// From project instructions section 5.2
	do {
		pipeline.retire();
		pipeline.writeback();
		pipeline.execute();
		pipeline.issue();
		pipeline.dispatch();
		pipeline.registerRead();
		pipeline.rename();
		pipeline.decode();
		pipeline.fetch();
	} while (advanceCycle());
}

void Simulator::showResults()
{
	pipeline.printInstructions();
}

bool Simulator::advanceCycle()
{
	return false;
}
