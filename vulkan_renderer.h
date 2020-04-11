#include <iostream>
#include <stdexcept>
#include <cstdlib>
#include <array>
#include <vector>
#include <cstring>
#include <set>
#include <optional>
#include <cstdint>
#include <fstream>
#include <chrono>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// Include the glfw library and make it load the vulkan header file for you
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

using namespace std;

// QueueFamilies indices struct (contains index values to all the required queuefamilies)
//struct QueueFamilyIndices {
//	optional<uint32_t> graphicsFamily;
//	optional<uint32_t> presentFamily;
//
//	bool isComplete() {
//		return graphicsFamily.has_value() && presentFamily.has_value();
//	}
//};

//struct SwapChainSupportDetails {
//	VkSurfaceCapabilitiesKHR capabilities;
//	vector<VkSurfaceFormatKHR> formats;
//	vector<VkPresentModeKHR> presentModes;
//};

//struct Vertex {
//	glm::vec2 position;
//	glm::vec3 color;
//
//	static VkVertexInputBindingDescription getBindingDescription() {
//		VkVertexInputBindingDescription bindingDescription = {};
//		bindingDescription.binding = 0;
//		bindingDescription.stride = sizeof(Vertex);
//		bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
//
//		return bindingDescription;
//	}
//
//	static std::array<VkVertexInputAttributeDescription, 2> getAttributeDescriptions() {
//		std::array<VkVertexInputAttributeDescription, 2> attributeDescriptions = {};
//
//		attributeDescriptions[0].binding = 0;
//		attributeDescriptions[0].location = 0;
//		attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
//		attributeDescriptions[0].offset = offsetof(Vertex, position);
//
//		attributeDescriptions[1].binding = 0;
//		attributeDescriptions[1].location = 1;
//		attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
//		attributeDescriptions[1].offset = offsetof(Vertex, color);
//
//		return attributeDescriptions;
//	}
//};

//struct UniformBufferObject {
//	glm::mat4 model;
//	glm::mat4 view;
//	glm::mat4 proj;
//};

//class App {
//public:
//	void run();
//
//private:
//	GLFWwindow* window = nullptr;
//
//	VkInstance instance = VK_NULL_HANDLE;
//	VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
//	VkDevice logicalDevice = VK_NULL_HANDLE;
//	VkQueue graphicsQueue = VK_NULL_HANDLE;
//	VkSurfaceKHR surface = VK_NULL_HANDLE;
//	VkQueue presentQueue = VK_NULL_HANDLE;
//	VkSwapchainKHR swapChain = VK_NULL_HANDLE;
//	VkRenderPass renderPass = VK_NULL_HANDLE;
//	VkDescriptorSetLayout descriptorSetLayout = VK_NULL_HANDLE;
//	VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;
//	VkPipeline graphicsPipeline = VK_NULL_HANDLE;
//	VkCommandPool commandPool = VK_NULL_HANDLE;
//	VkBuffer vertexBuffer = VK_NULL_HANDLE;
//	VkDeviceMemory vertexBufferMemory = VK_NULL_HANDLE;
//	VkBuffer indexBuffer = VK_NULL_HANDLE;
//	VkDeviceMemory indexBufferMemory = VK_NULL_HANDLE;
//
//	std::vector<VkImage> swapChainImages;
//	VkFormat swapChainImageFormat;
//	VkExtent2D swapChainExtent;
//	std::vector<VkImageView> swapChainImageViews;
//	std::vector<VkFramebuffer> swapChainFramebuffers;
//	std::vector<VkCommandBuffer> commandBuffers;
//	std::vector<VkSemaphore> imageAvailableSemaphores;
//	std::vector<VkSemaphore> renderFinishedSemaphores;
//	std::vector<VkFence> inFlightFences;
//	std::vector<VkFence> imagesInFlight;
//	std::vector<VkBuffer> uniformBuffers;
//	std::vector<VkDeviceMemory> uniformBuffersMemory;
//
//	size_t currentFrame = 0;
//	bool framebufferResized = false;
//
//	void initWindow();
//	static void framebufferResizeCallback(GLFWwindow* window, int width, int height);
//	void initVulkan();
//	bool getValidationLayerSupport();
//	void createInstance();
//	QueueFamilyIndices getQueueFamilies(VkPhysicalDevice device);
//	SwapChainSupportDetails getSwapchainSupport(VkPhysicalDevice device);
//	VkSurfaceFormatKHR selectSwapSurfaceFormat(const vector<VkSurfaceFormatKHR>& surfaceFormats);
//	VkPresentModeKHR selectPresentMode(const vector<VkPresentModeKHR>& presentModes);
//	VkExtent2D selectSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);
//	bool isDeviceSuitable(VkPhysicalDevice device);
//	bool doesDeviceSupportExtensions(VkPhysicalDevice device);
//	void createSurface();
//	void selectPhysicalDevice();
//	void createLogicalDevice();
//	void createSwapChain();
//	void recreateSwapchain();
//	void cleanupSwapchain();
//	void createImageViews();
//	static vector<char> readFile(const string& filename);
//	VkShaderModule createShaderModule(const vector<char>& shaderCodeBytes);
//	void createRenderPass();
//	void createDescriptorSetLayout();
//	void createGraphicsPipeline();
//	void createFrameBuffers();
//	void createCommandPool();
//	uint32_t getMemoryType(uint32_t typefilter, VkMemoryPropertyFlags properties);
//	void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);
//	void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
//	void createVertexBuffer();
//	void createIndexBuffer();
//	void createUniformBuffers();
//	void createCommandBuffers();
//	void createSyncObjects();
//	void updateUniformBuffer(uint32_t currentImage);
//	void drawFrame();
//	void mainLoop();
//	void cleanup();
//};