/*
    Copyright (c) Aleksey Fedotov
    MIT license
*/

#include "Camera.h"
#include <glm/gtc/matrix_transform.hpp>

auto Camera::setPerspective(float fov, float aspectRatio, float nearClip, float farClip) -> Camera&
{
    this->fov_ = fov;
    this->aspectRatio_ = aspectRatio;
    this->nearClip_ = nearClip;
    this->farClip_ = farClip;
    ortho_ = false;
    return *this;
}

auto Camera::setOrthographic(float width, float height, float nearClip, float farClip) -> Camera&
{
    this->nearClip_ = nearClip;
    this->farClip_ = farClip;
    orthoWidth_ = width;
    orthoHeight_ = height;
    ortho_ = true;
    return *this;
}

auto Camera::projMatrix() const -> const glm::mat4
{
    return ortho_
        ? glm::ortho(orthoWidth_, orthoHeight_, nearClip_, farClip_)
        : glm::perspective(fov_, aspectRatio_, nearClip_, farClip_);
}
