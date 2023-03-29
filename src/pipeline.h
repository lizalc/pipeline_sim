// Donavan Lance (dlance)
// ECE 463 Fall 2018

#ifndef SIM_PIPELINE_H
#define SIM_PIPELINE_H

#include "instruction.h"
#include "pipeline_register.h"
#include <memory>
#include <unordered_map>

class Pipeline {
public:
	Pipeline(unsigned long robSize, unsigned long IQSize, unsigned long width);

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
	std::unordered_map<PipelineRegister, std::unique_ptr<RegisterBase>> registers;

	// ROB?
	// Cycle counters?
};

#endif  // SIM_PIPELINE_H
