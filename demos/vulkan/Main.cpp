/*
    Copyright (c) Aleksey Fedotov
    MIT license
*/

#include "common/vk/VulkanAppBase.h"
#include "common/vk/VulkanCmdBuffer.h"
#include <imgui.h>
#include <examples/imgui_impl_sdl.h>
#include <examples/imgui_impl_vulkan.h>

class App: public vk::AppBase
{
public:
	App(): vk::AppBase(1366, 768, false)
	{
	}

protected:
    vk::CmdBuffer cmdBuf_;
    vk::Resource<VkSemaphore> completeSemaphore_;
    vk::Resource<VkDescriptorPool> uiDescPool_;
    vk::RenderPass uiRenderPass_;
    VkSemaphore waitSemaphore_ = nullptr;

	void init() override
	{
        cmdBuf_ = vk::CmdBuffer(device());
        completeSemaphore_ = vk::createSemaphore(device());

        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO(); (void)io;

        window()->onProcessEvent([](auto &evt)
        {
            ImGui_ImplSDL2_ProcessEvent(&evt);
        });

        ImGui_ImplSDL2_InitForVulkan(window()->sdlWindow());

        // Init desc pool
        {
            std::vector<VkDescriptorPoolSize> poolSizes = {
                { VK_DESCRIPTOR_TYPE_SAMPLER, 1000 },
                { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 },
                { VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000 },
                { VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000 },
                { VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000 },
                { VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000 },
                { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000 },
                { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000 },
                { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000 },
                { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000 },
                { VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000 }
            };

            VkDescriptorPoolCreateInfo poolInfo = {};
            poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
            poolInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
            poolInfo.maxSets = 1000 * poolSizes.size();
            poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
            poolInfo.pPoolSizes = poolSizes.data();

            uiDescPool_ = vk::Resource<VkDescriptorPool>{device(), vkDestroyDescriptorPool};
            vk::assertResult(vkCreateDescriptorPool(device(), &poolInfo, nullptr, uiDescPool_.cleanRef()));
        }

        // Render pass
        {
            const auto cfg = vk::RenderPassConfig()
                .addColorAttachment(device().colorFormat(), VK_IMAGE_LAYOUT_GENERAL); // TODO proper layout
            uiRenderPass_ = vk::RenderPass(device(), cfg);
        }

        // Init render pass
        {
            ImGui_ImplVulkan_InitInfo initInfo = {};
            initInfo.Instance = window()->instance();
            initInfo.PhysicalDevice = device().physical();
            initInfo.Device = device();
            initInfo.QueueFamily = device().queueIndex();
            initInfo.Queue = device().queue();
            initInfo.PipelineCache = VK_NULL_HANDLE;
            initInfo.DescriptorPool = uiDescPool_;
            initInfo.Allocator = nullptr;
            initInfo.MinImageCount = 2;
            initInfo.ImageCount = swapchain().imageCount();
            initInfo.CheckVkResultFn = [](VkResult) {};

            ImGui_ImplVulkan_Init(&initInfo, uiRenderPass_.handle());
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
        waitSemaphore_ = swapchain().moveNext();

        const auto fb = swapchain().currentFrameBuffer();
        auto &renderPass = swapchain().renderPass();
        const auto canvasWidth = window()->canvasWidth();
        const auto canvasHeight = window()->canvasHeight();

        const VkClearRect clearRect{{{0, 0}, {static_cast<uint32_t>(canvasWidth), static_cast<uint32_t>(canvasHeight)}}, 0, 1};
        const VkClearValue clearValue{{{1, 0, 0, 1}}};
        const glm::vec4 viewport{0, 0, canvasWidth, canvasHeight};

        cmdBuf_.begin(false)
            .beginRenderPass(renderPass, fb, canvasWidth, canvasHeight)
            .clearColorAttachment(0, clearValue, clearRect)
            .setViewport(viewport, 0, 1)
            .setScissor(viewport)
	        .endRenderPass()
	        .end();

        vk::queueSubmit(device().queue(), 1, &waitSemaphore_, 1, &completeSemaphore_, 1, cmdBuf_);
        vk::assertResult(vkQueueWaitIdle(device().queue()));

        waitSemaphore_ = completeSemaphore_;

        swapchain().present(device().queue(), 1, &waitSemaphore_);
        vk::assertResult(vkQueueWaitIdle(device().queue()));
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