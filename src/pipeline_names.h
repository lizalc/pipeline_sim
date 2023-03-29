// Donavan Lance (dlance)
// ECE 463 Fall 2018
// Project 3 - Superscalar Pipeline Simulator

#ifndef SIM_PIPELINE_NAMES_H
#define SIM_PIPELINE_NAMES_H

enum PipelineStage {
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

enum PipelineRegister { DE, RN, RR, DI, IQ, execute_list, WB, ROB };

enum InstructionRegister { Destination, Source1, Source2 };

#endif  // SIM_PIPELINE_NAMES_H
