// Donavan Lance (dlance)
// ECE 463 Fall 2018

#ifndef SIM_PIPELINE_H
#define SIM_PIPELINE_H

#include "instruction.h"
#include <vector>

class Pipeline {
public:
	Pipeline();

	void fetch();
	void decode();
	void rename();
	void registerRead();
	void dispatch();
	void issue();
	void execute();
	void writeback();
	void retire();

	void setInstructionCache(std::vector<Instruction> &&instructions);
	void printInstructions() const;

private:
	// Project assumes perfect caches and branch prediction, so
	// just load all instructions into the instruction cache.
	std::vector<Instruction> instructionCache;

	// Array / vector of pipeline registers?
	// ROB?
	// Cycle counters?
};

#endif  // SIM_PIPELINE_H
