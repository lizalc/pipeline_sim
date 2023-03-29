// Donavan Lance (dlance)
// ECE 463 Fall 2018
// Project 3 - Superscalar Pipeline Simulator

#ifndef SIM_PIPELINE_NAMES_H
#define SIM_PIPELINE_NAMES_H

enum class PipelineStage {
	Fetch,
	Decode,
	Rename,
	RegisterRead,
	Dispatch,
	Issue,
	Execute,
	Writeback,
	Retire
};

enum class InstructionRegisters { Destination, Source1, Source2 };

#endif  // SIM_PIPELINE_NAMES_H
