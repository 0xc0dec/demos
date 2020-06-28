/*
    Copyright (c) Aleksey Fedotov
    MIT license
*/

#include "common/Camera.h"
#include "common/Spectator.h"
#include "common/vk/VulkanAppBase.h"
#include "common/vk/VulkanCmdBuffer.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class App final : public vk::AppBase
{
public:
	App(): vk::AppBase(1366, 768, false)
	{
	}

private:
    vk::CmdBuffer cmdBuf_;

    struct
    {
        vk::Resource<VkSemaphore> complete;
        VkSemaphore wait = nullptr;
    } semaphores_;

	Camera camera_;
	Transform root_;
	Transform t1_, t2_, t3_;

	void init() override
	{
        cmdBuf_ = vk::CmdBuffer(device());
        semaphores_.complete = vk::createSemaphore(device());
	}

	void render() override
	{
		applySpectator(camera_.transform(), *window());
		
		const auto dt = window()->timeDelta();
		const auto deltaAngle = glm::radians(100 * dt);
		root_.rotate({0, 1, 0}, deltaAngle, TransformSpace::World);
		t1_.rotate({0, 0, 1}, deltaAngle, TransformSpace::Self);
		t2_.rotate({0, 0, 1}, deltaAngle, TransformSpace::Self);
		t3_.rotate({0, 1, 0}, deltaAngle, TransformSpace::Parent);

        const auto canvasWidth = window()->canvasWidth();
        const auto canvasHeight = window()->canvasHeight();

        const VkClearRect clearRect{{{0, 0}, {static_cast<uint32_t>(canvasWidth), static_cast<uint32_t>(canvasHeight)}}, 0, 1};
        const VkClearValue clearValue{{{0, 0.5f, 0.6f, 1}}};
        const glm::vec4 viewport{0, 0, canvasWidth, canvasHeight};

        cmdBuf_.begin(false)
            .beginRenderPass(swapchain().renderPass(), swapchain().currentFrameBuffer(), canvasWidth, canvasHeight)
            .clearColorAttachment(0, clearValue, clearRect)
            .setViewport(viewport, 0, 1)
            .setScissor(viewport)
            .endRenderPass()
            .end();

        semaphores_.wait = swapchain().moveNext();
        vk::queueSubmit(device().queue(), 1, &semaphores_.wait, 1, &semaphores_.complete, 1, cmdBuf_);

        swapchain().present(device().queue(), 1, &semaphores_.complete);
        vk::ensure(vkQueueWaitIdle(device().queue()));
	}

	void cleanup() override
	{
	}
};

int main()
{
	App().run();
	return 0;
}
