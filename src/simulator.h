// Donavan Lance (dlance)
// ECE 463 Fall 2018

#ifndef SIM_SIMULATOR_H
#define SIM_SIMULATOR_H

#include "pipeline.h"
#include <string>

class Simulator {
public:
	Simulator(int robSize, int IQSize, int width, const std::string &traceFile);

	void setup();
	void run();
	void showResults();

private:
	Pipeline pipeline;
	int robSize, IQSize, width;
	const std::string traceFile;

	// Belong here or in pipeline?
	bool advanceCycle();
};

#endif  // SIM_SIMULATOR_H
