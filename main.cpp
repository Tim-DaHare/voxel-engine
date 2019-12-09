
// Include the glfw library and make it load the vulkan header file
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <iostream>
#include <stdexcept>
#include <functional>
#include <cstdlib>
#include <vector>

const int WIDTH = 800;
const int HEIGHT = 600;

class App {
public:
	void run() {
		initWindow();
		initVulkan();
		mainLoop();
		cleanup();
	}

private:
	GLFWwindow* window;
	VkInstance instance;

	void initWindow() {
		glfwInit();
		
		// Stop glfw from creating a OpenGL context automatically
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		// Make the window not resizable
		glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

		window = glfwCreateWindow(WIDTH, HEIGHT, "Voxel Engine", nullptr, nullptr);
	}

	void initVulkan() {
		createInstance();
	}

	void createInstance() {
		VkApplicationInfo appInfo = {};
		appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		appInfo.pApplicationName = "Voxel Engine";
		appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.pEngineName = "No Engine";
		appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.apiVersion = VK_API_VERSION_1_0;

		uint32_t extensionCount = 0;
		std::vector<VkExtensionProperties> extensions(extensionCount);
		vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());

		uint32_t glfwExtensionCount = 0;
		const char** glfwExtensions;
		glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

		// Todo: make a function that checks if all the required glfw extensions are in the enumerated instance extensions.

		VkInstanceCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		createInfo.pApplicationInfo = &appInfo;
		createInfo.enabledExtensionCount = glfwExtensionCount;
		createInfo.ppEnabledExtensionNames = glfwExtensions;
		createInfo.enabledLayerCount = 0;
		
		if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS) {
			throw new std::runtime_error("failed to create vkInstance");
		}

	}

	void mainLoop() {
		while (!glfwWindowShouldClose(window)) {
			glfwPollEvents();
		}
	}

	void cleanup() {
		vkDestroyInstance(instance, nullptr);

		glfwDestroyWindow(window);
		glfwTerminate();
	}
};

int main() {
	App app;

	try {
		app.run();
	}
	catch (const std::exception& e) {
		std::cerr << e.what() << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}