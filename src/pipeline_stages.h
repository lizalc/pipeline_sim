// Donavan Lance (dlance)
// ECE 463 Fall 2018
// Project 3 - Superscalar Pipeline Simulator

#ifndef SIM_PIPELINE_STAGES_H
#define SIM_PIPELINE_STAGES_H

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

#endif  // SIM_PIPELINE_STAGES_H
