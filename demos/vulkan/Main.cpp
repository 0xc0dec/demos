/*
    Copyright (c) Aleksey Fedotov
    MIT license
*/

#include "common/vk/VulkanAppBase.h"
#include "common/vk/VulkanCmdBuffer.h"

class App: public vk::AppBase
{
public:
	App(): vk::AppBase(1366, 768, false)
	{
	}

protected:
    vk::CmdBuffer cmdBuf_;
    vk::Resource<VkSemaphore> completeSemaphore_;
    VkSemaphore waitSemaphore_;

	void init() override
	{
        cmdBuf_ = vk::CmdBuffer(device());
        completeSemaphore_ = vk::createSemaphore(device());
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
		
	}
};

void main()
{
	App().run();
}