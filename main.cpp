#include "app.hpp"

/* Standard Library */
#include <iostream>
#include <stdexcept>

int main() {
	vk::App app;
	try {
		app.run();
	} catch (const std::exception &e) {
		std::cerr << e.what() << std::endl;
		return EXIT_FAILURE;
	}

	std::cout << "Hello, World!" << std::endl;
	return 0;
}