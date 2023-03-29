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
	Pipeline(int robSize, int IQSize, int width);

	void fetch();
	void decode();
	void rename();
	void registerRead();
	void dispatch();
	void issue();
	void execute();
	void writeback();
	void retire();

	void advanceCycle();
	void addToInstructionCache(std::shared_ptr<Instruction> instruction);
	bool instructionsFinished() const;
	void printInstructions() const;

private:
	// Project assumes perfect caches and branch prediction, so
	// just load all instructions into the instruction cache. Also serves
	// as a way to go back through the processed instructions to display
	// final data.
	std::vector<std::shared_ptr<Instruction>> instructionCache;
	std::unordered_map<PipelineRegister, std::unique_ptr<RegisterBase>> registers;
	std::unordered_map<int, int> renameMapTable;

	// Pipeline width
	int width;
	// Current overall cycle
	int overallCycle;
	// For getting the current instruction from the instruction cache
	size_t currentIndex;

	// ROB?
	// Cycle counters?
};

#endif  // SIM_PIPELINE_H
