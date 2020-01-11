/*
    Copyright (c) Aleksey Fedotov
    MIT license
*/

#include "Spectator.h"
#include "Transform.h"
#include "Device.h"
#include <glm/gtx/vector_angle.hpp>

// Moves the given transform as a "spectator", i.e. flying first-person camera.
void applySpectator(Transform &transform, Device &device, float mouseSensitivity, float movementSpeed)
{
	const auto mouseMotion = device.getMouseMotion();
	const auto dt = device.getTimeDelta();

    if (device.isMouseButtonDown(SDL_BUTTON_RIGHT, true))
        device.setCursorCaptured(true);
    if (device.isMouseButtonReleased(SDL_BUTTON_RIGHT))
        device.setCursorCaptured(false);

    if (device.isMouseButtonDown(SDL_BUTTON_RIGHT, false))
    {
        if (mouseMotion.x != 0)
            transform.rotate({0, 1, 0}, mouseSensitivity * -mouseMotion.x, TransformSpace::World);

        if (mouseMotion.y != 0)
        {
	        const auto angleToUp = glm::angle(glm::normalize(transform.localForwardDir()), glm::vec3(0, 1, 0));
            auto delta = mouseSensitivity * -mouseMotion.y;
            if (delta > 0)
            {
                if (angleToUp - delta <= 0.1f)
                    delta = angleToUp - 0.1f;
            }
            else
            {
                if (angleToUp - delta >= 3.04f)
                    delta = angleToUp - 3.04f;
            }

            transform.rotate({1, 0, 0}, delta, TransformSpace::Self);
        }
    }

    auto movement = glm::vec3(0);
    if (device.isKeyPressed(SDLK_w, false))
        movement += transform.localForwardDir();
    if (device.isKeyPressed(SDLK_s, false))
        movement += transform.localBackDir();
    if (device.isKeyPressed(SDLK_a, false))
        movement += transform.localLeftDir();
    if (device.isKeyPressed(SDLK_d, false))
        movement += transform.localRightDir();
    if (device.isKeyPressed(SDLK_q, false))
        movement += transform.localDownDir();
    if (device.isKeyPressed(SDLK_e, false))
        movement += transform.localUpDir();
    movement = dt * movementSpeed * glm::normalize(movement);

    if (!glm::any(glm::isnan(movement)))
        transform.translateLocal(movement);
}