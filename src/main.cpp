// Donavan Lance (dlance)
// ECE 463 Fall 2018
// Project 3 - Superscalar Pipeline Simulator

#include "simulator.h"
#include <iostream>
#include <string>

int main(const int argc, const char *argv[])
{
	if (argc != 5) {
		std::cout << "Error: Wrong number of inputs:" << argc - 1 << '\n';
		return EXIT_FAILURE;
	}

	Simulator simulator{std::stoul(argv[1]), std::stoul(argv[2]),
	                    std::stoul(argv[3]), argv[4]};

	simulator.setup();
	simulator.run();
	simulator.showResults();

	return 0;
}
