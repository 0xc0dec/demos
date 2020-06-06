/*
    Copyright (c) Aleksey Fedotov
    MIT license
*/

#pragma once

#include "VulkanResource.h"
#include <unordered_map>

namespace vk
{
	class Device
	{
	public:
		Device() = default;
		Device(VkInstance instance, VkSurfaceKHR surface);
		Device(Device &&other) = default;
		Device(const Device &other) = delete;
		~Device() = default;

		bool isFormatSupported(VkFormat format, VkFormatFeatureFlags features) const;
		auto gpuName() const -> const char* { return physicalProperties_.deviceName; }

		auto operator=(const Device &other) -> Device& = delete;
		auto operator=(Device &&other) -> Device& = default;

		operator VkDevice() const { return handle_; }

		auto handle() const -> VkDevice { return handle_; }
		auto surface() const -> VkSurfaceKHR { return surface_; }
		auto physical() const -> VkPhysicalDevice { return physical_; }
		auto physicalFeatures() const -> VkPhysicalDeviceFeatures { return physicalFeatures_; }
		auto physicalProperties() const -> VkPhysicalDeviceProperties { return physicalProperties_; }
		auto physicalMemoryFeatures() const -> VkPhysicalDeviceMemoryProperties { return physicalMemoryFeatures_; }
		auto colorFormat() const -> VkFormat { return colorFormat_; }
		auto depthFormat() const -> VkFormat { return depthFormat_; }
		auto colorSpace() const -> VkColorSpaceKHR { return colorSpace_; }
		auto commandPool() const -> VkCommandPool { return commandPool_; }
		auto queue() const -> VkQueue { return queue_; }
		auto queueIndex() const -> uint32_t { return queueIndex_; }

	private:
		Resource<VkDevice> handle_;
		VkSurfaceKHR surface_ = nullptr;
		Resource<VkCommandPool> commandPool_;
		VkPhysicalDevice physical_ = nullptr;
		VkPhysicalDeviceFeatures physicalFeatures_{};
		VkPhysicalDeviceProperties physicalProperties_{};
		VkPhysicalDeviceMemoryProperties physicalMemoryFeatures_{};
		VkFormat colorFormat_ = VK_FORMAT_UNDEFINED;
		VkFormat depthFormat_ = VK_FORMAT_UNDEFINED;
		VkColorSpaceKHR colorSpace_ = VK_COLOR_SPACE_MAX_ENUM_KHR;
		VkQueue queue_ = nullptr;
		uint32_t queueIndex_ = -1;
		Resource<VkDebugReportCallbackEXT> debugCallback_;
		std::unordered_map<VkFormat, VkFormatFeatureFlags> supportedFormats_;

		void selectPhysicalDevice(VkInstance instance);
		void detectFormatSupport(VkFormat format);
		auto selectDepthFormat() const -> VkFormat;
	};
}
