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
    auto transform() -> Transform& { return transform_; }

    auto setPerspective(float fov, float aspectRatio, float nearClip, float farClip) -> Camera&;
    auto setOrthographic(float width, float height, float nearClip, float farClip) -> Camera&;

    auto viewMatrix() const -> const glm::mat4 { return glm::inverse(transform_.worldMatrix()); }
    auto invViewMatrix() const -> const glm::mat4 { return glm::inverse(viewMatrix()); }
    auto projMatrix() const -> const glm::mat4;
    auto viewProjMatrix() const -> const glm::mat4 { return projMatrix() * viewMatrix(); }
    auto invViewProjMatrix() const -> const glm::mat4 { return glm::inverse(viewProjMatrix()); }

protected:
    bool ortho_ = false;
    float fov_ = glm::degrees(60.0f);
    float aspectRatio_ = 1;
    float orthoWidth_ = 1;
    float orthoHeight_ = 1;
    float nearClip_ = 1;
    float farClip_ = 100;
    
    Transform transform_;
};
