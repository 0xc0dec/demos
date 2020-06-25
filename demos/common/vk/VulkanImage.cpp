/*
    Copyright (c) Aleksey Fedotov
    MIT license
*/

#include "VulkanImage.h"
#include "VulkanCmdBuffer.h"
#include "VulkanDevice.h"
#include "VulkanBuffer.h"

using namespace vk;

static auto createImage(VkDevice device, VkFormat format, uint32_t width, uint32_t height, uint32_t mipLevels,
    uint32_t arrayLayers, VkImageCreateFlags createFlags, VkImageUsageFlags usageFlags) -> Resource<VkImage>
{
    VkImageCreateInfo imageCreateInfo{};
    imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
    imageCreateInfo.format = format;
    imageCreateInfo.mipLevels = mipLevels;
    imageCreateInfo.arrayLayers = arrayLayers;
    imageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    imageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
    imageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    imageCreateInfo.extent = {width, height, 1};
    imageCreateInfo.usage = usageFlags;
    imageCreateInfo.flags = createFlags;

    Resource<VkImage> image{device, vkDestroyImage};
    vk::assertResult(vkCreateImage(device, &imageCreateInfo, nullptr, image.cleanRef()));

    return image;
}

static auto allocateImageMemory(VkDevice device, VkPhysicalDeviceMemoryProperties memProps, VkImage image) -> Resource<VkDeviceMemory>
{
    VkMemoryRequirements memReqs{};
    vkGetImageMemoryRequirements(device, image, &memReqs);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memReqs.size;
    allocInfo.memoryTypeIndex = vk::findMemoryType(memProps, memReqs.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    Resource<VkDeviceMemory> memory{device, vkFreeMemory};
    vk::assertResult(vkAllocateMemory(device, &allocInfo, nullptr, memory.cleanRef()));
    vk::assertResult(vkBindImageMemory(device, image, memory, 0));

    return memory;
}

// TODO Refactor, reduce copy-paste
auto Image::empty(const Device &dev, uint32_t width, uint32_t height, VkFormat format, bool depth) -> Image
{
    const auto layout = VK_IMAGE_LAYOUT_GENERAL; // TODO better
    const auto usage = VK_IMAGE_USAGE_SAMPLED_BIT |
        (depth
            ? VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT
            : VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT
        );
    const auto aspect = depth ? VK_IMAGE_ASPECT_DEPTH_BIT : VK_IMAGE_ASPECT_COLOR_BIT;

    // TODO Better check. Checking for color attachment and sampled bits seems not right or too general
    panicIf(!dev.isFormatSupported(format, VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT |
        (depth ? VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT : VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT)),
        "Image format/features not supported"
    );

    auto image = Image(dev, width, height, 1, 1, format, layout, 0, usage, VK_IMAGE_VIEW_TYPE_2D, aspect);

    VkImageSubresourceRange range{};
    range.aspectMask = image.aspectMask_;
    range.baseMipLevel = 0;
    range.levelCount = 1;
    range.layerCount = 1;

    const auto barrier = vk::makeImagePipelineBarrier(
        image.image_,
        VK_IMAGE_LAYOUT_UNDEFINED,
        layout,
        range
    );

    CmdBuffer(dev)
        .begin(true)
        .putImagePipelineBarrier(
            VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
            VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
            barrier)
        .endAndFlush();

    return image;
}

// TODO Refactor, reduce copy-paste
auto Image::fromData(const Device &dev, uint32_t width, uint32_t height, uint32_t size, VkFormat format, void *data, bool generateMipmaps) -> Image
{
    const auto layout = VK_IMAGE_LAYOUT_GENERAL;
    auto usage =
        VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT |
        VK_IMAGE_USAGE_SAMPLED_BIT |
        VK_IMAGE_USAGE_TRANSFER_DST_BIT;

    panicIf(!dev.isFormatSupported(format,
        VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT |
        VK_FORMAT_FEATURE_TRANSFER_DST_BIT_KHR),
        "Image format/features not supported"
    );

    uint32_t mipLevels = 1;
    if (generateMipmaps)
    {
        panicIf(!dev.isFormatSupported(format, VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT),
            "Image format/features not supported"
        );
        mipLevels = static_cast<uint32_t>(std::floorf(std::log2f((std::fmax)(static_cast<float>(width), static_cast<float>(height))))) + 1;
        usage |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
    }

    auto image = Image(dev, width, height, mipLevels, 1, format, layout, 0, usage,
        VK_IMAGE_VIEW_TYPE_2D, VK_IMAGE_ASPECT_COLOR_BIT);

    VkImageSubresourceRange range{};
    range.aspectMask = image.aspectMask_;
    range.baseArrayLayer = 0;
    range.baseMipLevel = 0;
    range.levelCount = 1;
    range.layerCount = 1;

    const auto barrier = vk::makeImagePipelineBarrier(
        image.image_,
        VK_IMAGE_LAYOUT_UNDEFINED,
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        range
    );

    const auto srcBuf = Buffer::staging(dev, size, data);

    auto initCmdBuf = CmdBuffer(dev);
    initCmdBuf.begin(true);
    initCmdBuf.putImagePipelineBarrier(
        VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
        VK_PIPELINE_STAGE_TRANSFER_BIT,
        barrier
    );

    initCmdBuf.copyBuffer(srcBuf, image);

    if (generateMipmaps)
    {
        initCmdBuf.putImagePipelineBarrier(
            VK_PIPELINE_STAGE_TRANSFER_BIT,
            VK_PIPELINE_STAGE_TRANSFER_BIT,
            vk::makeImagePipelineBarrier(
                image.image_,
                VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                range
            )
        );
        initCmdBuf.endAndFlush();

        auto blitCmdBuf = CmdBuffer(dev);
        blitCmdBuf.begin(true);

        for (uint32_t i = 1; i < mipLevels; i++)
        {
            VkImageBlit imageBlit{};                

            // Source
            imageBlit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            imageBlit.srcSubresource.layerCount = 1;
            imageBlit.srcSubresource.mipLevel = i - 1;
            imageBlit.srcOffsets[1].x = static_cast<int>(width >> (i - 1));
            imageBlit.srcOffsets[1].y = static_cast<int>(height >> (i - 1));
            imageBlit.srcOffsets[1].z = 1;

            // Destination
            imageBlit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            imageBlit.dstSubresource.layerCount = 1;
            imageBlit.dstSubresource.mipLevel = i;
            imageBlit.dstOffsets[1].x = static_cast<int>(width >> i);
            imageBlit.dstOffsets[1].y = static_cast<int>(height >> i);
            imageBlit.dstOffsets[1].z = 1;

            VkImageSubresourceRange mipSubRange{};
            mipSubRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            mipSubRange.baseMipLevel = i;
            mipSubRange.levelCount = 1;
            mipSubRange.layerCount = 1;

            blitCmdBuf.putImagePipelineBarrier(
                VK_PIPELINE_STAGE_TRANSFER_BIT,
                VK_PIPELINE_STAGE_TRANSFER_BIT,
                vk::makeImagePipelineBarrier(
                    image.image_,
                    VK_IMAGE_LAYOUT_UNDEFINED,
                    VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                    mipSubRange
                )
            );

            blitCmdBuf.blit(
                image.image_,
                image.image_,
                VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                imageBlit,
                VK_FILTER_LINEAR
            );

            blitCmdBuf.putImagePipelineBarrier(
                VK_PIPELINE_STAGE_TRANSFER_BIT,
                VK_PIPELINE_STAGE_TRANSFER_BIT,
                vk::makeImagePipelineBarrier(
                    image.image_,
                    VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                    VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                    mipSubRange
                )
            );
        }

        range.levelCount = static_cast<uint32_t>(mipLevels);

        blitCmdBuf.putImagePipelineBarrier(
            VK_PIPELINE_STAGE_TRANSFER_BIT,
            VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
            vk::makeImagePipelineBarrier(
                image.image_,
                VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                layout,
                range
            )
        );

        blitCmdBuf.endAndFlush();
    }
    else
    {
        initCmdBuf.putImagePipelineBarrier(
            VK_PIPELINE_STAGE_TRANSFER_BIT,
            VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
            vk::makeImagePipelineBarrier(
                image.image_,
                VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                layout,
                range
            )
        );
        initCmdBuf.endAndFlush();
    }

    return image;
}

auto Image::swapchainDepthStencil(const Device &dev, uint32_t width, uint32_t height, VkFormat format) -> Image
{
    return Image(dev, width, height, 1, 1, format,
        VK_IMAGE_LAYOUT_UNDEFINED,
        0,
        VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT,
        VK_IMAGE_VIEW_TYPE_2D,
        VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT);
}

Image::Image(const Device &dev, uint32_t width, uint32_t height, uint32_t mipLevels, uint32_t layers, VkFormat format, VkImageLayout layout,
    VkImageCreateFlags createFlags, VkImageUsageFlags usageFlags, VkImageViewType viewType, VkImageAspectFlags aspectMask):
    layout_(layout),
    format_(format),
    mipLevels_(mipLevels),
    width_(width),
    height_(height),
    aspectMask_(aspectMask)
{
    image_ = createImage(dev.handle(), format, width, height, mipLevels, layers, createFlags, usageFlags);
    memory_ = allocateImageMemory(dev.handle(), dev.physicalMemoryFeatures(), image_);
    view_ = vk::createImageView(dev.handle(), format, viewType, mipLevels, layers, image_, aspectMask);
}
