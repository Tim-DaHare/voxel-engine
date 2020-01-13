#include <iostream>
#include <stdexcept>
#include <cstdlib>
#include <vector>
#include <cstring>
#include <set>
#include <optional>
#include <cstdint>
#include <fstream>

// Include the glfw library and make it load the vulkan header file for you
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

using namespace std;

// Resolution
const int WIDTH = 800;
const int HEIGHT = 600;

const int MAX_FRAMES_IN_FLIGHT = 2;


const std::vector<const char*> validationLayers = {
	"VK_LAYER_KHRONOS_validation"
};

#ifdef NDEBUG
const bool enableValidationLayers = false;
#else
const bool enableValidationLayers = true;
#endif

// required vulkan extensions
const vector<const char*> requiredVulkanExtensions = {
	VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

// QueueFamilies indices struct (contains index values to all the required queuefamilies)
struct QueueFamilyIndices {
	optional<uint32_t> graphicsFamily;
	optional<uint32_t> presentFamily;

	bool isComplete() {
		return graphicsFamily.has_value() && presentFamily.has_value();
	}
};

struct SwapChainSupportDetails {
	VkSurfaceCapabilitiesKHR capabilities;
	vector<VkSurfaceFormatKHR> formats;
	vector<VkPresentModeKHR> presentModes;
};

class App {
public:
	void run() {
		initWindow();
		initVulkan();
		mainLoop();
		cleanup();
	}

private:
	GLFWwindow* window = nullptr;

	VkInstance instance = VK_NULL_HANDLE;
	VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
	VkDevice logicalDevice = VK_NULL_HANDLE;
	VkQueue graphicsQueue = VK_NULL_HANDLE;
	VkSurfaceKHR surface = VK_NULL_HANDLE;
	VkQueue presentQueue = VK_NULL_HANDLE;
	VkSwapchainKHR swapChain = VK_NULL_HANDLE;
	VkRenderPass renderPass = VK_NULL_HANDLE;
	VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;
	VkPipeline graphicsPipeline = VK_NULL_HANDLE;
	VkCommandPool commandPool = VK_NULL_HANDLE;
	
	std::vector<VkImage> swapChainImages;
	VkFormat swapChainImageFormat;
	VkExtent2D swapChainExtent;
	std::vector<VkImageView> swapChainImageViews;
	std::vector<VkFramebuffer> swapChainFramebuffers;
	std::vector<VkCommandBuffer> commandBuffers;
	std::vector<VkSemaphore> imageAvailableSemaphores;
	std::vector<VkSemaphore> renderFinishedSemaphores;
	std::vector<VkFence> inFlightFences;
	std::vector<VkFence> imagesInFlight;

	size_t currentFrame = 0;

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
		// Create surface before selecting a physical device(GPU) because it may influince gpu selection.
		createSurface();
		selectPhysicalDevice();
		createLogicalDevice();
		createSwapChain();
		createImageViews();
		createRenderPass();
		createGraphicsPipeline();
		createFrameBuffers();
		createCommandPool();
		createCommandBuffers();
		createSyncObjects();
	}

	bool getValidationLayerSupport() {
		uint32_t layerCount;
		vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
		vector<VkLayerProperties> availableLayers(layerCount);
		vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

		for (const char* validationLayer : validationLayers) {
			bool layerFound = false;

			for (const auto& availableLayer : availableLayers) {
				if (strcmp(validationLayer, availableLayer.layerName) == 0) {
					layerFound = true;
					break;
				}
			}

			if (!layerFound) {
				return false;
			}
		}

		return true;
	}

	void createInstance() {
		if (enableValidationLayers && !getValidationLayerSupport()) {
			throw runtime_error("Validation layers requested but are not supported");
		}

		if (!glfwVulkanSupported()) {
			throw std::runtime_error("Vulkan is not supported on this machine, please check if your drivers installed and are up to date.");
		}

		VkApplicationInfo appInfo = {};
		appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		appInfo.pApplicationName = "Voxel Engine";
		appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.pEngineName = "No Engine";
		appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.apiVersion = VK_API_VERSION_1_0;

		uint32_t glfwExtensionCount = 0;
		const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

		VkInstanceCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		createInfo.pApplicationInfo = &appInfo;
		createInfo.enabledExtensionCount = glfwExtensionCount;
		createInfo.ppEnabledExtensionNames = glfwExtensions;
		if (enableValidationLayers) {
			createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
			createInfo.ppEnabledLayerNames = validationLayers.data();
		} else {
			createInfo.enabledLayerCount = 0;
		}
		
		
		if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS) {
			throw std::runtime_error("failed to create vkInstance.");
		}
	}

	QueueFamilyIndices getQueueFamilies(VkPhysicalDevice device) {

		uint32_t queueFamilyCount = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);
		vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

		QueueFamilyIndices indices = {};

		int i = 0;
		// Search and set graphics queue family
		for (const auto& queueFamily : queueFamilies) {
			// Check if the bit representing graphics support is set to 1
			if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
				indices.graphicsFamily = i;
			}

			VkBool32 presentSupport = false;
			vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);

			if (presentSupport) {
				indices.presentFamily = i;
			}

			// Stop looping if we have loaded all required queueFamilies
			if (indices.isComplete()) {
				break;
			}
			i++;
		}

		return indices;
	}

	SwapChainSupportDetails getSwapchainSupport(VkPhysicalDevice device) {
		SwapChainSupportDetails supportDetails = {};

		// Get surface capabilities
		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &supportDetails.capabilities);

		// Get surface formats
		uint32_t formatCount = 0;
		vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);
		if (formatCount) {
			supportDetails.formats.resize(formatCount);
			vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, supportDetails.formats.data());
		}

		// Get surface present modes
		uint32_t presentmodeCount = 0;
		vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentmodeCount, nullptr);
		if (presentmodeCount) {
			supportDetails.presentModes.resize(presentmodeCount);
			vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentmodeCount, supportDetails.presentModes.data());
		}

		return supportDetails;
	}

	VkSurfaceFormatKHR selectSwapSurfaceFormat(const vector<VkSurfaceFormatKHR>& surfaceFormats) {
		for (const auto& format : surfaceFormats) {
			if (format.colorSpace == VK_COLORSPACE_SRGB_NONLINEAR_KHR && format.format == VK_FORMAT_B8G8R8A8_UNORM) {
				return format;
			}
		}

		return surfaceFormats[0];
	}

	VkPresentModeKHR selectPresentMode(const vector<VkPresentModeKHR>& presentModes) {
		// Select mailbox triple buffering if available
		for (const auto& presentMode : presentModes) {
			if (presentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
				return presentMode;
			}
		}

		// Return default vSync if mailbox tripple buffering is not available (it is guaranteed to be available always)
		return VK_PRESENT_MODE_FIFO_KHR;
	}

	VkExtent2D selectSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities) {

		if (capabilities.currentExtent.width != UINT32_MAX) {
			// For now just return the extent with the same dimensions as the window
			return capabilities.currentExtent;
		}

		int width, height;
		glfwGetFramebufferSize(window, &width, &height);

		VkExtent2D actualExtent = {

		};
	}

	bool isDeviceSuitable(VkPhysicalDevice device) {
		QueueFamilyIndices indeces = getQueueFamilies(device);
		bool extensionsSupported = doesDeviceSupportExtensions(device);

		// Only check if swapchain is suitable if the swapchain in the requiredvulkanextensions is actually supported
		bool isSwapchainSuitable = false;
		if (extensionsSupported) {
			SwapChainSupportDetails details = getSwapchainSupport(device);
			isSwapchainSuitable = !details.formats.empty() && !details.presentModes.empty();
		}
		
		return indeces.isComplete() && extensionsSupported && isSwapchainSuitable;
	}

	bool doesDeviceSupportExtensions(VkPhysicalDevice device) {
		uint32_t extensionCount = 0;
		vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);
		vector<VkExtensionProperties> availableExtensions(extensionCount);
		vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

		std::set<std::string> remainingMissingExtensions(requiredVulkanExtensions.begin(), requiredVulkanExtensions.end());
		for (const auto& extension : availableExtensions) {
			remainingMissingExtensions.erase(extension.extensionName);
		}

		// if the set is empty it means there are no missing extensions, thus the device supports them.
		return remainingMissingExtensions.empty();
	}

	void createSurface() {
		if (glfwCreateWindowSurface(instance, window, nullptr, &surface) != VK_SUCCESS) {
			throw runtime_error("Failed to create vulkan window surface.");
		}
	}

	void selectPhysicalDevice() {
		uint32_t deviceCount = 0;
		vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);

		if (deviceCount == 0) {
			throw std::runtime_error("This machine has no devices with vulkan support.");
		}

		vector<VkPhysicalDevice> devices(deviceCount);
		vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

		// select the first suitable device
		for (const auto& device : devices) {
			if (isDeviceSuitable(device)) {
				physicalDevice = device;
				break;
			}
		}

		if (physicalDevice == VK_NULL_HANDLE) {
			throw runtime_error("This machine has no devices that support all requirements.");
		}
	}

	void createLogicalDevice() {
		QueueFamilyIndices indices = getQueueFamilies(physicalDevice);
		float queuePriority = 1.0f;

		vector<VkDeviceQueueCreateInfo> queueCreateInfos;
		set<uint32_t> uniqueQueueFamilies = {
			indices.graphicsFamily.value(),
			indices.presentFamily.value()
		};

		for (uint32_t queueFamily : uniqueQueueFamilies) {
			VkDeviceQueueCreateInfo queueCreateInfo = {};
			queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			queueCreateInfo.queueFamilyIndex = queueFamily;
			queueCreateInfo.queueCount = 1;
			queueCreateInfo.pQueuePriorities = &queuePriority;

			queueCreateInfos.push_back(queueCreateInfo);
		}

		// Specify required device features here later
		VkPhysicalDeviceFeatures deviceFeatures = {};

		VkDeviceCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		createInfo.pQueueCreateInfos = queueCreateInfos.data();
		createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
		createInfo.enabledExtensionCount = static_cast<uint32_t>(requiredVulkanExtensions.size());
		createInfo.ppEnabledExtensionNames = requiredVulkanExtensions.data();
		// Enable validation layers later if implemented
		createInfo.enabledLayerCount = 0;

		if (vkCreateDevice(physicalDevice, &createInfo, nullptr, &logicalDevice) != VK_SUCCESS) {
			throw runtime_error("Failed to create logical device.");
		}

		// set all queue handlers as class members
		vkGetDeviceQueue(logicalDevice, indices.graphicsFamily.value(), 0, &graphicsQueue);
		vkGetDeviceQueue(logicalDevice, indices.presentFamily.value(), 0, &presentQueue);
	}

	void createSwapChain() {
		SwapChainSupportDetails details = getSwapchainSupport(physicalDevice);

		VkSurfaceFormatKHR surfaceFormat = selectSwapSurfaceFormat(details.formats);
		VkPresentModeKHR presentMode = selectPresentMode(details.presentModes);
		VkExtent2D extent = selectSwapExtent(details.capabilities);

		// Buffer one image more than neccesary in advance in the swap chain to avoid stutters
		uint32_t imageCount = details.capabilities.minImageCount + 1;
		// limit the amount of images to the maximum allowed amount specified in the swapchain capabilities struct
		uint32_t maxImageCount = details.capabilities.maxImageCount;
		if (maxImageCount && imageCount > maxImageCount) {
			imageCount = details.capabilities.maxImageCount;
		}

		VkSwapchainCreateInfoKHR createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		createInfo.surface = surface;
		createInfo.minImageCount = imageCount;
		createInfo.imageFormat = surfaceFormat.format;
		createInfo.imageColorSpace = surfaceFormat.colorSpace;
		createInfo.imageExtent = extent;
		createInfo.imageArrayLayers = 1;
		// use VK_IMAGE_USAGE_TRANSFER_DST_BIT  if you want to use post processing effects later (this usage will copy a manipulated image to the swapchain after processing)
		createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

		// note: This may change later to be more dynamic if i decide to transfer ownership of an image to multiple queuefamilies (if applicable) manually
		QueueFamilyIndices indices = getQueueFamilies(physicalDevice);
		uint32_t queueFamilyIndicies[] = {
			indices.graphicsFamily.value(),
			indices.presentFamily.value()
		};
		if (indices.graphicsFamily != indices.presentFamily) {
			createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
			createInfo.queueFamilyIndexCount = 2;
			createInfo.pQueueFamilyIndices = queueFamilyIndicies;
		} else {
			createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		}
		// endnote

		createInfo.preTransform = details.capabilities.currentTransform;
		createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		createInfo.presentMode = presentMode;
		createInfo.clipped = VK_TRUE;

		// Reference to the old swapchain for when the window gets resized, currently null because the window is not resizable
		createInfo.oldSwapchain = VK_NULL_HANDLE;

		if (vkCreateSwapchainKHR(logicalDevice, &createInfo, nullptr, &swapChain) != VK_SUCCESS) {
			throw runtime_error("Failed to create swap chain.");
		}

		vkGetSwapchainImagesKHR(logicalDevice, swapChain, &imageCount, nullptr);
		swapChainImages.resize(imageCount);
		vkGetSwapchainImagesKHR(logicalDevice, swapChain, &imageCount, swapChainImages.data());

		swapChainImageFormat = surfaceFormat.format;
		swapChainExtent = extent;
	}

	void recreateSwapchain() {
		vkDeviceWaitIdle(logicalDevice);

		cleanupSwapchain();

		createSwapChain();
		createImageViews();
		createRenderPass();
		createGraphicsPipeline();
		createFrameBuffers();
		createCommandBuffers();
	}

	void cleanupSwapchain() {
		for (size_t i = 0; i < swapChainFramebuffers.size(); i++) {
			vkDestroyFramebuffer(logicalDevice, swapChainFramebuffers[i], nullptr);
		}

		vkFreeCommandBuffers(logicalDevice, commandPool, static_cast<uint32_t>(commandBuffers.size()), commandBuffers.data());

		vkDestroyPipeline(logicalDevice, graphicsPipeline, nullptr);
		vkDestroyPipelineLayout(logicalDevice, pipelineLayout, nullptr);
		vkDestroyRenderPass(logicalDevice, renderPass, nullptr);

		for (auto imageView : swapChainImageViews) {
			vkDestroyImageView(logicalDevice, imageView, nullptr);
		}

		vkDestroySwapchainKHR(logicalDevice, swapChain, nullptr);
	}

	void createImageViews() {
		swapChainImageViews.resize(swapChainImages.size());
		for (size_t i = 0; i < swapChainImages.size(); i++) {
			VkImageViewCreateInfo createInfo = {};
			createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			createInfo.image = swapChainImages[i];
			createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
			createInfo.format = swapChainImageFormat;
			createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			createInfo.subresourceRange.baseMipLevel = 0;
			createInfo.subresourceRange.levelCount = 1;
			createInfo.subresourceRange.baseArrayLayer = 0;
			createInfo.subresourceRange.layerCount = 1;

			if (vkCreateImageView(logicalDevice, &createInfo, nullptr, &swapChainImageViews[i]) != VK_SUCCESS) {
				throw runtime_error("Failed to create image views");
			}
		}
	}

	static vector<char> readFile(const string& filename) {
		ifstream file(filename, ios::binary | ios::ate);

		if (!file.is_open()) {
			throw runtime_error("Failed to open file: " + filename);
		}

		size_t fileSize = (size_t)file.tellg();
		vector<char> buffer(fileSize);

		file.seekg(0);
		file.read(buffer.data(), fileSize);

		file.close();

		return buffer;
	}

	VkShaderModule createShaderModule(const vector<char>& shaderCodeBytes) {
		VkShaderModuleCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		createInfo.codeSize = shaderCodeBytes.size();
		createInfo.pCode = reinterpret_cast<const uint32_t*>(shaderCodeBytes.data());

		VkShaderModule shaderModule;
		if (vkCreateShaderModule(logicalDevice, &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
			throw runtime_error("Failed to create shader module.");
		}

		return shaderModule;
	}

	void createRenderPass() {
		VkAttachmentDescription colorAttachment = {};
		colorAttachment.format = swapChainImageFormat;
		// No multisampling yet so set sample count to 1
		colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
		colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

		VkAttachmentReference colorAttachmentRef = {};
		colorAttachmentRef.attachment = 0;
		colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		VkSubpassDescription subpass = {};
		subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpass.colorAttachmentCount = 1;
		subpass.pColorAttachments = &colorAttachmentRef;

		VkSubpassDependency dependency = {};
		dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
		dependency.dstSubpass = 0;
		dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependency.srcAccessMask = 0;
		dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

		VkRenderPassCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		createInfo.attachmentCount = 1;
		createInfo.pAttachments = &colorAttachment;
		createInfo.subpassCount = 1;
		createInfo.pSubpasses = &subpass;
		createInfo.dependencyCount = 1;
		createInfo.pDependencies = &dependency;

		if (vkCreateRenderPass(logicalDevice, &createInfo, nullptr, &renderPass) != VK_SUCCESS) {
			throw runtime_error("Failed to create render pass.");
		}
	}

	void createGraphicsPipeline() {
		vector<char> vertShaderCode = readFile("shaders/bin/vert.spv");
		vector<char> fragShaderCode = readFile("shaders/bin/frag.spv");

		VkShaderModule vertShaderModule = createShaderModule(vertShaderCode);
		VkShaderModule fragShaderModule = createShaderModule(fragShaderCode);

		VkPipelineShaderStageCreateInfo vertCreateInfo = {};
		vertCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		vertCreateInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
		vertCreateInfo.module = vertShaderModule;
		vertCreateInfo.pName = "main";

		VkPipelineShaderStageCreateInfo fragCreateInfo = {};
		fragCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		fragCreateInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
		fragCreateInfo.module = fragShaderModule;
		fragCreateInfo.pName = "main";

		VkPipelineShaderStageCreateInfo shaderStages[] = {
			fragCreateInfo,
			vertCreateInfo
		};

		VkPipelineVertexInputStateCreateInfo vertexInputInfo = {};
		vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		vertexInputInfo.vertexBindingDescriptionCount = 0;
		vertexInputInfo.pVertexBindingDescriptions = nullptr;
		vertexInputInfo.vertexAttributeDescriptionCount = 0;
		vertexInputInfo.pVertexAttributeDescriptions = nullptr;

		VkPipelineInputAssemblyStateCreateInfo inputAsseblyInfo = {};
		inputAsseblyInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		inputAsseblyInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		inputAsseblyInfo.primitiveRestartEnable = VK_FALSE;
		
		VkViewport viewPort = {};
		viewPort.x = 0;
		viewPort.y = 0;
		viewPort.width = swapChainExtent.width;
		viewPort.height = swapChainExtent.height;
		viewPort.minDepth = 0;
		viewPort.maxDepth = 1;

		VkRect2D scissor = {};
		scissor.offset = { 0, 0 };
		scissor.extent = swapChainExtent;

		VkPipelineViewportStateCreateInfo viewportStateInfo = {};
		viewportStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		viewportStateInfo.viewportCount = 1;
		viewportStateInfo.pViewports = &viewPort;
		viewportStateInfo.scissorCount = 1;
		viewportStateInfo.pScissors = &scissor;

		VkPipelineRasterizationStateCreateInfo rasterizerInfo = {};
		rasterizerInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		rasterizerInfo.depthBiasEnable = VK_FALSE;
		rasterizerInfo.rasterizerDiscardEnable = VK_FALSE;
		rasterizerInfo.polygonMode = VK_POLYGON_MODE_FILL;
		rasterizerInfo.lineWidth = 1;
		rasterizerInfo.cullMode = VK_CULL_MODE_BACK_BIT; // maybe this is wrong???
		rasterizerInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;
		rasterizerInfo.depthBiasEnable = VK_FALSE;
		// rasterizerInfo.depthBiasConstantFactor = 0.0f; // Optional
		// rasterizerInfo.depthBiasClamp = 0.0f; // Optional
		// rasterizerInfo.depthBiasSlopeFactor = 0.0f; // Optional

		// Not used yet
		VkPipelineMultisampleStateCreateInfo multisamplingInfo = {};
		multisamplingInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		multisamplingInfo.sampleShadingEnable = VK_FALSE;
		multisamplingInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
		// multisamplingInfo.minSampleShading = 1.0f; // Optional
		// multisamplingInfo.pSampleMask = nullptr; // Optional
		// multisamplingInfo.alphaToCoverageEnable = VK_FALSE; // Optional
		// multisamplingInfo.alphaToOneEnable = VK_FALSE; // Optional

		VkPipelineColorBlendAttachmentState colorBlendAttachment = {};
		colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
		// Enable/disable alpha color blending
		colorBlendAttachment.blendEnable = VK_TRUE;
		// Comment the following colorblendattachment config lines if you disable alpha color blending
		colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
		colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
		colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
		colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
		colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
		colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;

		VkPipelineColorBlendStateCreateInfo colorBlendInfo = {};
		colorBlendInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		// Set logicOpEnable to true for bitwise combination blending
		colorBlendInfo.logicOpEnable = VK_FALSE;
		// colorBlendInfo.logicOp = VK_LOGIC_OP_COPY; // Optional
		colorBlendInfo.attachmentCount = 1;
		colorBlendInfo.pAttachments = &colorBlendAttachment;
		// colorBlendInfo.blendConstants[0] = 0.0f; // Optional
		// colorBlendInfo.blendConstants[1] = 0.0f; // Optional
		// colorBlendInfo.blendConstants[2] = 0.0f; // Optional
		// colorBlendInfo.blendConstants[3] = 0.0f; // Optional

		VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		// pipelineLayoutInfo.setLayoutCount = 0; // Optional
		// pipelineLayoutInfo.pSetLayouts = nullptr; // Optional
		// pipelineLayoutInfo.pushConstantRangeCount = 0; // Optional
		// pipelineLayoutInfo.pPushConstantRanges = nullptr; // Optional

		if (vkCreatePipelineLayout(logicalDevice, &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS) {
			throw runtime_error("Failed to create pipeline layout.");
		}

		VkGraphicsPipelineCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		createInfo.stageCount = 2;
		createInfo.pStages = shaderStages;
		createInfo.pVertexInputState = &vertexInputInfo;
		createInfo.pViewportState = &viewportStateInfo;
		createInfo.pInputAssemblyState = &inputAsseblyInfo;
		createInfo.pRasterizationState = &rasterizerInfo;
		createInfo.pMultisampleState = &multisamplingInfo;
		createInfo.pColorBlendState = &colorBlendInfo;
		createInfo.pDepthStencilState = nullptr; // Optional
		createInfo.pDynamicState = nullptr; // Optional
		createInfo.layout = pipelineLayout;
		createInfo.renderPass = renderPass;
		createInfo.subpass = 0;
		createInfo.basePipelineHandle = VK_NULL_HANDLE; // Optional
		createInfo.basePipelineIndex = -1; // Optional

		if (vkCreateGraphicsPipelines(logicalDevice, VK_NULL_HANDLE, 1, &createInfo, nullptr, &graphicsPipeline) != VK_SUCCESS) {
			throw runtime_error("Failed to create graphics pipeline(s).");
		}

		vkDestroyShaderModule(logicalDevice, fragShaderModule, nullptr);
		vkDestroyShaderModule(logicalDevice, vertShaderModule, nullptr);
	}

	void createFrameBuffers() {
		swapChainFramebuffers.resize(swapChainImageViews.size());
		for (size_t i = 0; i < swapChainImageViews.size(); i++) {
			VkImageView attachments[] = {
				swapChainImageViews[i]
			};

			VkFramebufferCreateInfo createInfo = {};
			createInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
			createInfo.renderPass = renderPass;
			createInfo.attachmentCount = 1;
			createInfo.pAttachments = attachments;
			createInfo.width = swapChainExtent.width;
			createInfo.height = swapChainExtent.height;
			createInfo.layers = 1;

			if (vkCreateFramebuffer(logicalDevice, &createInfo, nullptr, &swapChainFramebuffers[i]) != VK_SUCCESS) {
				throw runtime_error("Failed to create framebuffer for imageview " + i);
			}
		}
	}

	void createCommandPool() {
		QueueFamilyIndices queueFamilyIndices = getQueueFamilies(physicalDevice);

		VkCommandPoolCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		createInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();
		createInfo.flags = 0;

		if (vkCreateCommandPool(logicalDevice, &createInfo, nullptr, &commandPool) != VK_SUCCESS) {
			throw runtime_error("failed to create command pool.");
		}
	}

	void createCommandBuffers() {
		commandBuffers.resize(swapChainFramebuffers.size());
		
		VkCommandBufferAllocateInfo allocInfo = {};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.commandPool = commandPool;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());

		if (vkAllocateCommandBuffers(logicalDevice, &allocInfo, commandBuffers.data()) != VK_SUCCESS) {
			throw runtime_error("failed to allocate command buffers.");
		}

		for (size_t i = 0; i < commandBuffers.size(); i++) {
			VkCommandBufferBeginInfo beginInfo = {};
			beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
			beginInfo.flags = 0;
			// beginInfo.pInheritanceInfo = nullptr;

			if (vkBeginCommandBuffer(commandBuffers[i], &beginInfo) != VK_SUCCESS) {
				throw runtime_error("failed to begin command buffer recording for buffer: " + i);
			}

			VkClearValue clearColor = { 0.0f, 0.0f, 0.0f, 1.0f }; // Black

			VkRenderPassBeginInfo renderPassInfo = {};
			renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
			renderPassInfo.renderPass = renderPass;
			renderPassInfo.framebuffer = swapChainFramebuffers[i];
			renderPassInfo.renderArea.offset = { 0, 0 };
			renderPassInfo.renderArea.extent = swapChainExtent;
			renderPassInfo.clearValueCount = 1;
			renderPassInfo.pClearValues = &clearColor;

			vkCmdBeginRenderPass(commandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

			vkCmdBindPipeline(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);

			vkCmdDraw(commandBuffers[i], 3, 1, 0, 0);

			vkCmdEndRenderPass(commandBuffers[i]);

			if (vkEndCommandBuffer(commandBuffers[i]) != VK_SUCCESS) {
				throw runtime_error("failed to record the command buffer for buffer: " + i);
			}
		}
	}

	void createSyncObjects() {
		imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
		renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
		inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);
		imagesInFlight.resize(swapChainImages.size(), VK_NULL_HANDLE);

		VkSemaphoreCreateInfo semaphoreInfo = {};
		semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

		VkFenceCreateInfo fenceInfo = {};
		fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
			VkResult createRenderFinishedSemaphoreResult = vkCreateSemaphore(logicalDevice, &semaphoreInfo, nullptr, &renderFinishedSemaphores[i]);
			VkResult createImageAvailableSemaphoreResult = vkCreateSemaphore(logicalDevice, &semaphoreInfo, nullptr, &imageAvailableSemaphores[i]);

			VkResult createFenceResult = vkCreateFence(logicalDevice, &fenceInfo, nullptr, &inFlightFences[i]);

			if (createRenderFinishedSemaphoreResult != VK_SUCCESS || createImageAvailableSemaphoreResult != VK_SUCCESS || createFenceResult != VK_SUCCESS) {
				throw runtime_error("Failed to create syncobjects for frame: " + i);
			}
		}
	}

	void drawFrame() {
		currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;

		uint32_t imageIndex;
		vkAcquireNextImageKHR(logicalDevice, swapChain, UINT64_MAX, imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &imageIndex);
		
		if (imagesInFlight[imageIndex] != VK_NULL_HANDLE) {
			vkWaitForFences(logicalDevice, 1, &imagesInFlight[imageIndex], VK_TRUE, UINT64_MAX);
		}

		imagesInFlight[imageIndex] = inFlightFences[currentFrame];

		VkSemaphore waitSemaphores[] = { imageAvailableSemaphores[currentFrame] };
		VkSemaphore signalSemaphores[] = { renderFinishedSemaphores[currentFrame] };
		VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };

		VkSubmitInfo submitInfo = {};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = waitSemaphores;
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = signalSemaphores;
		submitInfo.pWaitDstStageMask = waitStages;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &commandBuffers[imageIndex];

		vkResetFences(logicalDevice, 1, &inFlightFences[currentFrame]);

		if (vkQueueSubmit(graphicsQueue, 1, &submitInfo, inFlightFences[currentFrame]) != VK_SUCCESS) {
			throw runtime_error("Failed to submit draw command to queue.");
		}

		VkSwapchainKHR swapChains[] = { swapChain };

		VkPresentInfoKHR presentInfo = {};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores = signalSemaphores;
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = swapChains;
		presentInfo.pImageIndices = &imageIndex;

		vkQueuePresentKHR(presentQueue, &presentInfo);
	}

	void mainLoop() {
		while (!glfwWindowShouldClose(window)) {
			glfwPollEvents();
			drawFrame();
		}

		vkDeviceWaitIdle(logicalDevice); // My own idea to wait for the frames to be bufferd before exiting the application
	}

	void cleanup() {
		cleanupSwapchain();

		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
			vkDestroySemaphore(logicalDevice, renderFinishedSemaphores[i], nullptr);
			vkDestroySemaphore(logicalDevice, imageAvailableSemaphores[i], nullptr);
			vkDestroyFence(logicalDevice, inFlightFences[i], nullptr);
		}

		vkDestroyCommandPool(logicalDevice, commandPool, nullptr);
		vkDestroyDevice(logicalDevice, nullptr);
		vkDestroySurfaceKHR(instance, surface, nullptr);
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