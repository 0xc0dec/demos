/*
    Copyright (c) Aleksey Fedotov
    MIT license
*/

#include "Transform.h"
#include "Camera.h"
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_transform.inl>
#include <algorithm>

static const uint32_t dirtyBitLocal = 1;
static const uint32_t dirtyBitWorld = 1 << 1;
static const uint32_t dirtyBitInvTransposedWorld = 1 << 2;
static const uint32_t dirtyBitAll = dirtyBitLocal | dirtyBitWorld | dirtyBitInvTransposedWorld;

auto Transform::setParent(Transform *parent) -> Transform&
{
    if (parent != this && parent != this->parent_)
    {
        if (this->parent_)
        {
            auto &parentChildren = this->parent_->children_;
            this->parent_->children_.erase(std::remove(parentChildren.begin(), parentChildren.end(), this), parentChildren.end());
        }
        this->parent_ = parent;
        if (parent)
            parent->children_.push_back(this);
        setDirtyWithChildren(dirtyBitWorld | dirtyBitInvTransposedWorld);
    }

    return *this;
}

auto Transform::clearChildren() -> Transform&
{
    while (!children_.empty())
    {
        auto child = *children_.begin();
        child->setParent(nullptr);
    }

    return *this;
}

auto Transform::matrix() const -> glm::mat4
{
    if (dirtyFlags_ & dirtyBitLocal)
    {
        matrix_ = glm::translate(glm::mat4(1.0f), localPosition_) *
                 glm::mat4_cast(localRotation_) *
                 glm::scale(glm::mat4(1.0f), localScale_);
        dirtyFlags_ &= ~dirtyBitLocal;
    }

    return matrix_;
}

auto Transform::worldMatrix() const -> glm::mat4
{
    if (dirtyFlags_ & dirtyBitWorld)
    {
        if (parent_)
            worldMatrix_ = parent_->worldMatrix() * matrix();
        else
            worldMatrix_ = matrix();
        dirtyFlags_ &= ~dirtyBitWorld;
    }

    return worldMatrix_;
}

auto Transform::invTransposedWorldMatrix() const -> glm::mat4
{
    if (dirtyFlags_ & dirtyBitInvTransposedWorld)
    {
        invTransposedWorldMatrix_ = worldMatrix();
        invTransposedWorldMatrix_ = glm::transpose(glm::inverse(invTransposedWorldMatrix_));
        dirtyFlags_ &= ~dirtyBitInvTransposedWorld;
    }

    return invTransposedWorldMatrix_;
}

auto Transform::worldViewMatrix(const Camera &camera) const -> glm::mat4
{
    return camera.getViewMatrix() * worldMatrix();
}

auto Transform::worldViewProjMatrix(const Camera &camera) const -> glm::mat4
{
    return camera.getViewProjectionMatrix() * worldMatrix();
}

auto Transform::invTransposedWorldViewMatrix(const Camera &camera) const -> glm::mat4
{
    return glm::transpose(glm::inverse(camera.getViewMatrix() * worldMatrix()));
}

auto Transform::translateLocal(const glm::vec3 &translation) -> Transform&
{
    localPosition_ += translation;
    setDirtyWithChildren(dirtyBitAll);
    return *this;
}

auto Transform::rotate(const glm::quat &rotation, TransformSpace space) -> Transform&
{
    const auto normalizedRotation = glm::normalize(rotation);

    switch (space)
    {
        case TransformSpace::Self:
            localRotation_ = localRotation_ * normalizedRotation;
            break;
        case TransformSpace::Parent:
            localRotation_ = normalizedRotation * localRotation_;
            break;
        case TransformSpace::World:
        {
            const auto invWorldRotation = glm::inverse(worldRotation());
            localRotation_ = localRotation_ * invWorldRotation * normalizedRotation * worldRotation();
            break;
        }
        default:
            break;
    }

    setDirtyWithChildren(dirtyBitAll);

    return *this;
}

auto Transform::rotate(const glm::vec3 &axis, float angle, TransformSpace space) -> Transform&
{
	const auto rotation = glm::angleAxis(angle, axis);
    rotate(rotation, space);
    return *this;
}

auto Transform::scaleLocal(const glm::vec3 &scale) -> Transform&
{
    localScale_.x *= scale.x;
    localScale_.y *= scale.y;
    localScale_.z *= scale.z;
    setDirtyWithChildren(dirtyBitAll);
    return *this;
}

auto Transform::setLocalScale(const glm::vec3 &scale) -> Transform&
{
    localScale_ = scale;
    setDirtyWithChildren(dirtyBitAll);
    return *this;
}

auto Transform::lookAt(const glm::vec3 &target, const glm::vec3 &up) -> Transform&
{
    auto localTarget = glm::vec4(target, 1);
    auto localUp = glm::vec4(up, 0);

    if (parent_)
    {
        const auto m = glm::inverse(parent_->worldMatrix());
        localTarget = m * localTarget;
        localUp = m * localUp;
    }

	const auto lookAtMatrix = glm::inverse(glm::lookAt(localPosition_, glm::vec3(localTarget), glm::vec3(localUp)));
    setLocalRotation(glm::quat_cast(lookAtMatrix));

    return *this;
}

auto Transform::transformPoint(const glm::vec3 &point) const -> glm::vec3
{
    return matrix() * glm::vec4(point, 1.0f);
}

auto Transform::transformDirection(const glm::vec3 &direction) const -> glm::vec3
{
    return matrix() * glm::vec4(direction, 0);
}

auto Transform::setLocalRotation(const glm::quat &rotation) -> Transform&
{
    localRotation_ = rotation;
    setDirtyWithChildren(dirtyBitAll);
    return *this;
}

auto Transform::setLocalRotation(const glm::vec3 &axis, float angle) -> Transform&
{
    localRotation_ = glm::angleAxis(angle, axis);
    setDirtyWithChildren(dirtyBitAll);
    return *this;
}

auto Transform::setLocalPosition(const glm::vec3 &position) -> Transform&
{
    localPosition_ = position;
    setDirtyWithChildren(dirtyBitAll);
    return *this;
}

void Transform::setDirtyWithChildren(uint32_t flags)
{
    version_++;
    dirtyFlags_ |= flags;
    for (auto child : children_)
        child->setDirtyWithChildren(flags);
}

void Transform::setChildrenDirty(uint32_t flags)
{
    for (auto child : children_)
        child->setDirtyWithChildren(flags);
}

auto Transform::worldUpDir() const -> glm::vec3
{
    auto m = worldMatrix();
    return {-m[1][0], -m[1][1], -m[1][2]};
}

auto Transform::localUpDir() const -> glm::vec3
{
    auto m = matrix();
    return {m[1][0], m[1][1], m[1][2]};
}

auto Transform::worldDownDir() const -> glm::vec3
{
    auto m = worldMatrix();
    return {m[1][0], m[1][1], m[1][2]};
}

auto Transform::localDownDir() const -> glm::vec3
{
    auto m = matrix();
    return {-m[1][0], -m[1][1], -m[1][2]};
}

auto Transform::worldLeftDir() const -> glm::vec3
{
    auto m = worldMatrix();
    return {-m[0][0], -m[0][1], -m[0][2]};
}

auto Transform::localLeftDir() const -> glm::vec3
{
    auto m = matrix();
    return {-m[0][0], -m[0][1], -m[0][2]};
}

auto Transform::worldRightDir() const -> glm::vec3
{
    auto m = worldMatrix();
    return {m[0][0], m[0][1], m[0][2]};
}

auto Transform::localRightDir() const -> glm::vec3
{
    auto m = matrix();
    return {m[0][0], m[0][1], m[0][2]};
}

auto Transform::worldForwardDir() const -> glm::vec3
{
    auto m = worldMatrix();
    return {-m[2][0], -m[2][1], -m[2][2]};
}

auto Transform::localForwardDir() const -> glm::vec3
{
    auto m = matrix();
    return {-m[2][0], -m[2][1], -m[2][2]};
}

auto Transform::worldBackDir() const -> glm::vec3
{
    auto m = worldMatrix();
    return {m[2][0], m[2][1], m[2][2]};
}

auto Transform::localBackDir() const -> glm::vec3
{
    auto m = matrix();
    return {m[2][0], m[2][1], m[2][2]};
}
