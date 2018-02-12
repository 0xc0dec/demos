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
    if (parent != this && parent != this->parent)
    {
        if (this->parent)
        {
            auto &parentChildren = this->parent->children;
            this->parent->children.erase(std::remove(parentChildren.begin(), parentChildren.end(), this), parentChildren.end());
        }
        this->parent = parent;
        if (parent)
            parent->children.push_back(this);
        setDirtyWithChildren(dirtyBitWorld | dirtyBitInvTransposedWorld);
    }

    return *this;
}

auto Transform::clearChildren() -> Transform&
{
    while (!children.empty())
    {
        auto child = *children.begin();
        child->setParent(nullptr);
    }

    return *this;
}

auto Transform::getMatrix() const -> glm::mat4
{
    if (dirtyFlags & dirtyBitLocal)
    {
        matrix = glm::translate(glm::mat4(1.0f), localPosition) *
                 glm::mat4_cast(localRotation) *
                 glm::scale(glm::mat4(1.0f), localScale);
        dirtyFlags &= ~dirtyBitLocal;
    }

    return matrix;
}

auto Transform::getWorldMatrix() const -> glm::mat4
{
    if (dirtyFlags & dirtyBitWorld)
    {
        if (parent)
            worldMatrix = parent->getWorldMatrix() * getMatrix();
        else
            worldMatrix = getMatrix();
        dirtyFlags &= ~dirtyBitWorld;
    }

    return worldMatrix;
}

auto Transform::getInvTransposedWorldMatrix() const -> glm::mat4
{
    if (dirtyFlags & dirtyBitInvTransposedWorld)
    {
        invTransposedWorldMatrix = getWorldMatrix();
        invTransposedWorldMatrix = glm::transpose(glm::inverse(invTransposedWorldMatrix));
        dirtyFlags &= ~dirtyBitInvTransposedWorld;
    }

    return invTransposedWorldMatrix;
}

auto Transform::getWorldViewMatrix(const Camera &camera) const -> glm::mat4
{
    return camera.getViewMatrix() * getWorldMatrix();
}

auto Transform::getWorldViewProjMatrix(const Camera &camera) const -> glm::mat4
{
    return camera.getViewProjectionMatrix() * getWorldMatrix();
}

auto Transform::getInvTransposedWorldViewMatrix(const Camera &camera) const -> glm::mat4
{
    return glm::transpose(glm::inverse(camera.getViewMatrix() * getWorldMatrix()));
}

auto Transform::translateLocal(const glm::vec3 &translation) -> Transform&
{
    localPosition += translation;
    setDirtyWithChildren(dirtyBitAll);
    return *this;
}

auto Transform::rotate(const glm::quat &rotation, TransformSpace space) -> Transform&
{
    const auto normalizedRotation = glm::normalize(rotation);

    switch (space)
    {
        case TransformSpace::Self:
            localRotation = localRotation * normalizedRotation;
            break;
        case TransformSpace::Parent:
            localRotation = normalizedRotation * localRotation;
            break;
        case TransformSpace::World:
        {
            const auto invWorldRotation = glm::inverse(getWorldRotation());
            localRotation = localRotation * invWorldRotation * normalizedRotation * getWorldRotation();
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
    localScale.x *= scale.x;
    localScale.y *= scale.y;
    localScale.z *= scale.z;
    setDirtyWithChildren(dirtyBitAll);
    return *this;
}

auto Transform::setLocalScale(const glm::vec3 &scale) -> Transform&
{
    localScale = scale;
    setDirtyWithChildren(dirtyBitAll);
    return *this;
}

auto Transform::lookAt(const glm::vec3 &target, const glm::vec3 &up) -> Transform&
{
    auto localTarget = glm::vec4(target, 1);
    auto localUp = glm::vec4(up, 0);

    if (parent)
    {
        const auto m = glm::inverse(parent->getWorldMatrix());
        localTarget = m * localTarget;
        localUp = m * localUp;
    }

	const auto lookAtMatrix = glm::inverse(glm::lookAt(localPosition, glm::vec3(localTarget), glm::vec3(localUp)));
    setLocalRotation(glm::quat_cast(lookAtMatrix));

    return *this;
}

auto Transform::transformPoint(const glm::vec3 &point) const -> glm::vec3
{
    return getMatrix() * glm::vec4(point, 1.0f);
}

auto Transform::transformDirection(const glm::vec3 &direction) const -> glm::vec3
{
    return getMatrix() * glm::vec4(direction, 0);
}

auto Transform::setLocalRotation(const glm::quat &rotation) -> Transform&
{
    localRotation = rotation;
    setDirtyWithChildren(dirtyBitAll);
    return *this;
}

auto Transform::setLocalRotation(const glm::vec3 &axis, float angle) -> Transform&
{
    localRotation = glm::angleAxis(angle, axis);
    setDirtyWithChildren(dirtyBitAll);
    return *this;
}

auto Transform::setLocalPosition(const glm::vec3 &position) -> Transform&
{
    localPosition = position;
    setDirtyWithChildren(dirtyBitAll);
    return *this;
}

void Transform::setDirtyWithChildren(uint32_t flags)
{
    version++;
    dirtyFlags |= flags;
    for (auto child : children)
        child->setDirtyWithChildren(flags);
}

void Transform::setChildrenDirty(uint32_t flags)
{
    for (auto child : children)
        child->setDirtyWithChildren(flags);
}

auto Transform::getWorldUp() const -> glm::vec3
{
    auto m = getWorldMatrix();
    return {-m[1][0], -m[1][1], -m[1][2]};
}

auto Transform::getLocalUp() const -> glm::vec3
{
    auto m = getMatrix();
    return {m[1][0], m[1][1], m[1][2]};
}

auto Transform::getWorldDown() const -> glm::vec3
{
    auto m = getWorldMatrix();
    return {m[1][0], m[1][1], m[1][2]};
}

auto Transform::getLocalDown() const -> glm::vec3
{
    auto m = getMatrix();
    return {-m[1][0], -m[1][1], -m[1][2]};
}

auto Transform::getWorldLeft() const -> glm::vec3
{
    auto m = getWorldMatrix();
    return {-m[0][0], -m[0][1], -m[0][2]};
}

auto Transform::getLocalLeft() const -> glm::vec3
{
    auto m = getMatrix();
    return {-m[0][0], -m[0][1], -m[0][2]};
}

auto Transform::getWorldRight() const -> glm::vec3
{
    auto m = getWorldMatrix();
    return {m[0][0], m[0][1], m[0][2]};
}

auto Transform::getLocalRight() const -> glm::vec3
{
    auto m = getMatrix();
    return {m[0][0], m[0][1], m[0][2]};
}

auto Transform::getWorldForward() const -> glm::vec3
{
    auto m = getWorldMatrix();
    return {-m[2][0], -m[2][1], -m[2][2]};
}

auto Transform::getLocalForward() const -> glm::vec3
{
    auto m = getMatrix();
    return {-m[2][0], -m[2][1], -m[2][2]};
}

auto Transform::getWorldBack() const -> glm::vec3
{
    auto m = getWorldMatrix();
    return {m[2][0], m[2][1], m[2][2]};
}

auto Transform::getLocalBack() const -> glm::vec3
{
    auto m = getMatrix();
    return {m[2][0], m[2][1], m[2][2]};
}
