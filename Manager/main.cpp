
#include "StringIDImpl.hpp"
#include <string>
#include <iostream>
#include "Application.hpp"

int main(int argc, char **argv)
{
	auto start = std::chrono::high_resolution_clock::now();
	Application application;
	if (application.init(argc, argv) == false)
		return EXIT_FAILURE;
	application.run();
	
	auto end = std::chrono::high_resolution_clock::now();
	std::size_t elapsedTime = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
	
	std::cout << elapsedTime  << " micros" << std::endl;

	return EXIT_SUCCESS;
}