/*
    Copyright (c) Aleksey Fedotov
    MIT license
*/

#pragma once

#include "Transform.h"
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

class Camera final
{
public:
    auto getTransform() -> Transform& { return transform; }

    auto setPerspective(float fov, float aspectRatio, float nearClip, float farClip) -> Camera&;
    auto setOrthographic(float width, float height, float nearClip, float farClip) -> Camera&;

    auto getViewMatrix() const -> const glm::mat4 { return glm::inverse(transform.worldMatrix()); }
    auto getInvViewMatrix() const -> const glm::mat4 { return glm::inverse(getViewMatrix()); }
    auto getProjectionMatrix() const -> const glm::mat4;
    auto getViewProjectionMatrix() const -> const glm::mat4 { return getProjectionMatrix() * getViewMatrix(); }
    auto getInvViewProjectionMatrix() const -> const glm::mat4 { return glm::inverse(getViewProjectionMatrix()); }

protected:
    bool ortho = false;
    float fov = glm::degrees(60.0f);
    float aspectRatio = 1;
    float orthoWidth = 1;
    float orthoHeight = 1;
    float nearClip = 1;
    float farClip = 100;
    
    Transform transform;
};
