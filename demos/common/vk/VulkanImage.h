/**
 * Copyright (c) Aleksey Fedotov
 * MIT licence
 */

#pragma once

#include <glm/vec2.hpp>
#include "VulkanCommon.h"

namespace vk
{
    class Texture2DData;
    class CubeTextureData;
    class Device;

    class Image
    {
    public:
        static auto empty(const Device &dev, uint32_t width, uint32_t height, VkFormat format, bool depth) -> Image;
        static auto fromData(const Device &dev, uint32_t width, uint32_t height, uint32_t size, VkFormat format,
                             void *data, bool generateMipmaps) -> Image;
        static auto swapchainDepthStencil(const Device &dev, uint32_t width, uint32_t height, VkFormat format) -> Image; // TODO more generic?

        Image() = default;
        Image(const Image &other) = delete;
        Image(Image &&other) = default;

        auto format() const -> VkFormat { return format_; }
        auto size() const -> glm::vec2 { return {static_cast<float>(width_), static_cast<float>(height_)}; }
        auto mipLevels() const -> uint32_t { return mipLevels_; }
        auto layout() const -> VkImageLayout { return layout_; }
        auto view() const -> VkImageView { return view_; }
        auto handle() const -> VkImage { return image_; }
        auto width() const -> uint32_t { return width_; }
        auto height() const -> uint32_t { return height_; }

        auto operator=(const Image &other) -> Image & = delete;
        auto operator=(Image &&other) -> Image & = default;
        operator bool() const { return image_; }

    private:
        Resource<VkImage> image_;
        Resource<VkDeviceMemory> memory_;
        Resource<VkImageView> view_;
        VkImageLayout layout_ = VK_IMAGE_LAYOUT_UNDEFINED;
        VkFormat format_ = VK_FORMAT_UNDEFINED;
        uint32_t mipLevels_ = 0;
        uint32_t width_ = 0;
        uint32_t height_ = 0;
        VkImageAspectFlags aspectMask_ = VK_IMAGE_ASPECT_COLOR_BIT;

        Image(const Device &dev, uint32_t width, uint32_t height, uint32_t mipLevels, uint32_t layers, VkFormat format, VkImageLayout layout,
              VkImageCreateFlags createFlags, VkImageUsageFlags usageFlags, VkImageViewType viewType, VkImageAspectFlags aspectMask);
    };
}
