#include <iostream>
#include "opengl_renderer.h"
//#include "vulkan_renderer.h"


int main() {
	//App app;
	OpenGLRenderer openGLRenderer;

	try {
		//app.run();
		openGLRenderer.run();
	}
	catch (const std::exception& e) {
		std::cerr << e.what() << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}