// Donavan Lance (dlance)
// ECE 463 Fall 2018

#ifndef SIM_PIPELINE_H
#define SIM_PIPELINE_H

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

	// private:
	// Array / vector of pipeline registers?
	// ROB?
	// Cycle counters?
};

#endif  // SIM_PIPELINE_H
