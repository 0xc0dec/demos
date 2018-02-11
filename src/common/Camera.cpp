/*
    Copyright (c) Aleksey Fedotov
    MIT license
*/

#include "Camera.h"
#include <glm/gtc/matrix_transform.hpp>

auto Camera::setPerspective(float fov, float aspectRatio, float nearClip, float farClip) -> Camera&
{
    this->fov = fov;
    this->aspectRatio = aspectRatio;
    this->nearClip = nearClip;
    this->farClip = farClip;
    ortho = false;
    return *this;
}

auto Camera::setOrthographic(float width, float height, float nearClip, float farClip) -> Camera&
{
    this->nearClip = nearClip;
    this->farClip = farClip;
    orthoWidth = width;
    orthoHeight = height;
    ortho = true;
    return *this;
}

auto Camera::getProjectionMatrix() const -> const glm::mat4
{
    return ortho
        ? glm::ortho(orthoWidth, orthoHeight, nearClip, farClip)
        : glm::perspective(fov, aspectRatio, nearClip, farClip);
}
