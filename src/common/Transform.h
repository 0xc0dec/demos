/*
    Copyright (c) Aleksey Fedotov
    MIT license
*/

#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <vector>

class Camera;

enum class TransformSpace
{
    Self,
    Parent,
    World
};

class Transform final
{
public:
    // Can be used by anyone interested if a transform has changed. Goes from 0 to MAX_UINT, then wraps back.
    auto getVersion() const -> uint32_t { return version; }

    auto setParent(Transform *parent) -> Transform&;
    auto getParent() const -> Transform* { return parent; }

    auto getChild(uint32_t index) const -> Transform* { return children[index]; }
    auto getChildrenCount() const -> uint32_t { return children.size(); }
    auto clearChildren() -> Transform&;

    auto getLocalScale() const -> glm::vec3 { return localScale; }

    auto getWorldRotation() const -> glm::quat { return glm::quat_cast(getWorldMatrix()); }
    auto getLocalRotation() const -> glm::quat { return localRotation; }

    auto getWorldPosition() const -> glm::vec3 { return glm::vec3(getWorldMatrix()[3]); }
    auto getLocalPosition() const -> glm::vec3 { return localPosition; }

    auto getWorldUp() const -> glm::vec3;
    auto getLocalUp() const -> glm::vec3;

    auto getWorldDown() const -> glm::vec3;
    auto getLocalDown() const -> glm::vec3;

    auto getWorldLeft() const -> glm::vec3;
    auto getLocalLeft() const -> glm::vec3;

    auto getWorldRight() const -> glm::vec3;
    auto getLocalRight() const -> glm::vec3;

    auto getWorldForward() const -> glm::vec3;
    auto getLocalForward() const -> glm::vec3;

    auto getWorldBack() const -> glm::vec3;
    auto getLocalBack() const -> glm::vec3;

    auto translateLocal(const glm::vec3 &translation) -> Transform&;
    auto scaleLocal(const glm::vec3 &scale) -> Transform&;

    auto setLocalPosition(const glm::vec3 &position) -> Transform&;
    auto setLocalScale(const glm::vec3 &scale) -> Transform&;

    auto rotate(const glm::quat &rotation, TransformSpace space = TransformSpace::Self) -> Transform&;
    auto rotate(const glm::vec3 &axis, float angle, TransformSpace space = TransformSpace::Self) -> Transform&;

    auto setLocalRotation(const glm::quat &rotation) -> Transform&;
    auto setLocalRotation(const glm::vec3 &axis, float angle) -> Transform&;

    auto lookAt(const glm::vec3 &target, const glm::vec3 &up) -> Transform&;

    auto getMatrix() const -> glm::mat4;
    auto getWorldMatrix() const -> glm::mat4;
    auto getInvTransposedWorldMatrix() const -> glm::mat4;
    auto getWorldViewMatrix(const Camera &camera) const -> glm::mat4;
    auto getWorldViewProjMatrix(const Camera &camera) const -> glm::mat4;
    auto getInvTransposedWorldViewMatrix(const Camera &camera) const -> glm::mat4;

    auto transformPoint(const glm::vec3 &point) const -> glm::vec3;
    auto transformDirection(const glm::vec3 &direction) const -> glm::vec3;
    
private:
    uint32_t version = 0;
    mutable uint32_t dirtyFlags = ~0;

    Transform *parent = nullptr;
    std::vector<Transform *> children;

    glm::vec3 localPosition;
    glm::vec3 localScale{1, 1, 1};
    glm::quat localRotation;
    mutable glm::mat4 matrix;
    mutable glm::mat4 worldMatrix;
    mutable glm::mat4 invTransposedWorldMatrix;

    void setDirtyWithChildren(uint32_t flags);
    void setChildrenDirty(uint32_t flags);
};
