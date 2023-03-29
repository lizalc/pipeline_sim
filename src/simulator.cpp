// Donavan Lance (dlance)
// ECE 463 Fall 2018

#include "simulator.h"
#include "instruction.h"
#include <fstream>
#include <iomanip>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

Simulator::Simulator(const int robSize, const int IQSize, const int width,
                     const std::string &traceFile)
    : pipeline{robSize, IQSize, width},
      robSize{robSize},
      IQSize{IQSize},
      width{width},
      traceFile{traceFile}
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
		std::string pc;
		values >> pc;

		int opType, destReg, srcReg1, srcReg2;
		values >> opType >> destReg >> srcReg1 >> srcReg2;

		pipeline.addToInstructionCache(
		    std::make_shared<Instruction>(std::stoul(pc, nullptr, 16), sequenceNum,
		                                  opType, destReg, srcReg1, srcReg2));
		++sequenceNum;
	}
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

	std::cout << "# === Simulator Command =========\n";
	std::cout << "# ./sim " << robSize << " " << IQSize << " " << width << " "
	          << traceFile << '\n';
	std::cout << "#== = Processor Configuration == =\n";
	std::cout << "#ROB_SIZE = " << robSize << '\n';
	std::cout << "#IQ_SIZE = " << IQSize << '\n';
	std::cout << "#WIDTH = " << width << '\n';
	std::cout << "#== = Simulation Results == == == ==\n";
	std::cout << "#Dynamic Instruction Count = " << pipeline.getInstructionCount()
	          << '\n';
	std::cout << "#Cycles = " << pipeline.getTotalCycles() << '\n';
	std::cout << "#Instructions Per Cycle(IPC) = " << std::fixed
	          << std::setprecision(2)
	          << (static_cast<double>(pipeline.getInstructionCount()) /
	              pipeline.getTotalCycles())
	          << '\n';
}

bool Simulator::advanceCycle()
{
	pipeline.advanceCycle();
	// XXX -> advanceCycle likely needs to do more than this
	return pipeline.instructionsFinished();
}
