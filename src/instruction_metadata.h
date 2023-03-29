// Donavan Lance (dlance)
// ECE 463 Fall 2018
// Project 3 - Superscalar Pipeline Simulator

#ifndef SIM_INSTRUCTION_METADATA_H
#define SIM_INSTRUCTION_METADATA_H

#include "pipeline_names.h"
#include <unordered_map>
#include <utility>

class InstructionMetadata {
public:
	InstructionMetadata(int sequenceNum, int opType, int destReg, int srcReg1,
	                    int srcReg2);

	// Sets the cycle a stage started
	void initCycle(PipelineStage stage, int cycle);
	// Increments current stage cycle count.
	void updateCycle(PipelineStage stage);
	int dest() const;
	int src1() const;
	int src2() const;
	int op() const;
	int sequence() const;

private:
	const int sequenceNum;
	const int opType;
	int destReg, srcReg1, srcReg2;
	std::unordered_map<PipelineStage, std::pair<int, int>, std::hash<int>>
	    stageCycles;

	std::pair<int, int> operator[](PipelineStage stage) const;

	friend std::ostream &operator<<(std::ostream &stream,
	                                const InstructionMetadata &data);
};

#endif  // SIM_INSTRUCTION_METADATA_H
