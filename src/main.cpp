#include <iostream>

#include "Simulation.h"
#include "GlfwWindowManager.h"

int main()
{
	// Initialize Glfw
	auto scope = std::make_unique<GlfwWindowManager>();
	std::cout << "(main) Initialized Glfw." << "\n";

	// Start and wait for simulation
	auto simThread = Simulation::createSimulationThread();
	GlfwWindowManager::startEventLoop();
	std::cout << "Waiting for join..." << "\n";
	simThread.join();

	std::cout << "(main) Bye." << "\n";
	std::cout << std::flush;
}
