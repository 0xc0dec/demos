/**
 * Copyright (c) Aleksey Fedotov
 * MIT licence
 */

#include "common/vk/VulkanAppBase.h"
#include "common/vk/VulkanCmdBuffer.h"
#include <imgui.h>
#include <examples/imgui_impl_sdl.h>
#include <examples/imgui_impl_vulkan.h>

class App : public vk::AppBase
{
public:
    App() : vk::AppBase(1366, 768, false)
    {
    }

protected:
    vk::CmdBuffer cmdBuf_;

    struct
    {
        vk::Resource<VkSemaphore> complete;
        VkSemaphore wait = nullptr;
    } semaphores_;

    struct
    {
        vk::Resource<VkDescriptorPool> descPool;
    } ui_;

    void init() override
    {
        cmdBuf_ = vk::CmdBuffer(device());
        semaphores_.complete = vk::createSemaphore(device());

        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO &io = ImGui::GetIO();
        (void)io;

        window()->onProcessEvent([](auto &evt)
                                 { ImGui_ImplSDL2_ProcessEvent(&evt); });

        ImGui_ImplSDL2_InitForVulkan(window()->sdlWindow());

        {
            std::vector<VkDescriptorPoolSize> poolSizes = {
                {VK_DESCRIPTOR_TYPE_SAMPLER, 1000},
                {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000},
                {VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000},
                {VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000},
                {VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000},
                {VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000},
                {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000},
                {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000},
                {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000},
                {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000},
                {VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000}};

            VkDescriptorPoolCreateInfo poolInfo = {};
            poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
            poolInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
            poolInfo.maxSets = 1000 * poolSizes.size();
            poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
            poolInfo.pPoolSizes = poolSizes.data();

            ui_.descPool = vk::Resource<VkDescriptorPool>{device(), vkDestroyDescriptorPool};
            vk::ensure(vkCreateDescriptorPool(device(), &poolInfo, nullptr, ui_.descPool.cleanRef()));
        }

        {
            ImGui_ImplVulkan_InitInfo initInfo{};
            initInfo.Instance = window()->instance();
            initInfo.PhysicalDevice = device().physical();
            initInfo.Device = device();
            initInfo.QueueFamily = device().queueIndex();
            initInfo.Queue = device().queue();
            initInfo.PipelineCache = VK_NULL_HANDLE;
            initInfo.DescriptorPool = ui_.descPool;
            initInfo.Allocator = nullptr;
            initInfo.MinImageCount = 2;
            initInfo.ImageCount = swapchain().imageCount();
            initInfo.CheckVkResultFn = [](VkResult) {};

            ImGui_ImplVulkan_Init(&initInfo, swapchain().renderPass());
        }

        // Load fonts
        {
            auto cmdBuf = vk::CmdBuffer(device());
            cmdBuf.begin(true);
            ImGui_ImplVulkan_CreateFontsTexture(cmdBuf);
            cmdBuf.endAndFlush();
            ImGui_ImplVulkan_DestroyFontUploadObjects();
        }
    }

    void render() override
    {
        const auto canvasWidth = window()->canvasWidth();
        const auto canvasHeight = window()->canvasHeight();

        const VkClearRect clearRect{{{0, 0}, {static_cast<uint32_t>(canvasWidth), static_cast<uint32_t>(canvasHeight)}}, 0, 1};
        const VkClearValue clearValue{{{0, 0.5f, 0.6f, 1}}};
        const glm::vec4 viewport{0, 0, canvasWidth, canvasHeight};

        cmdBuf_.begin(false)
            .beginRenderPass(swapchain().renderPass(), swapchain().currentFrameBuffer(), canvasWidth, canvasHeight)
            .clearColorAttachment(0, clearValue, clearRect)
            .setViewport(viewport, 0, 1)
            .setScissor(viewport);

        ImGui_ImplSDL2_NewFrame(window()->sdlWindow());
        ImGui::NewFrame();

        auto open = true; // never close
        ImGui::ShowDemoWindow(&open);

        ImGui::Render();
        ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), cmdBuf_);

        cmdBuf_.endRenderPass()
            .end();

        semaphores_.wait = swapchain().moveNext();
        vk::queueSubmit(device().queue(), 1, &semaphores_.wait, 1, &semaphores_.complete, 1, cmdBuf_);

        swapchain().present(device().queue(), 1, &semaphores_.complete);
        vk::ensure(vkQueueWaitIdle(device().queue()));
    }

    void cleanup() override
    {
        ImGui_ImplVulkan_Shutdown();
        ImGui_ImplSDL2_Shutdown();
        ImGui::DestroyContext();
    }
};

void main()
{
    App().run();
}