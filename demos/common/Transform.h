/**
 * Copyright (c) Aleksey Fedotov
 * MIT licence
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

// Represents an object transform data - a point in space with local coordinate system.
class Transform final
{
public:
    // Can be used by anyone interested if a transform has changed. Goes from 0 to MAX_UINT, then wraps back.
    auto version() const -> uint32_t { return version_; }

    auto parent() const -> Transform * { return parent_; }
    auto setParent(Transform *parent) -> Transform &;

    auto child(uint32_t index) const -> Transform * { return children_[index]; }
    auto childrenCount() const -> uint32_t { return children_.size(); }
    auto clearChildren() -> Transform &;

    auto localScale() const -> glm::vec3 { return localScale_; }

    auto worldRotation() const -> glm::quat { return glm::quat_cast(worldMatrix()); }
    auto localRotation() const -> glm::quat { return localRotation_; }

    auto worldPosition() const -> glm::vec3 { return glm::vec3(worldMatrix()[3]); }
    auto localPosition() const -> glm::vec3 { return localPosition_; }

    auto worldUpDir() const -> glm::vec3;
    auto localUpDir() const -> glm::vec3;

    auto worldDownDir() const -> glm::vec3;
    auto localDownDir() const -> glm::vec3;

    auto worldLeftDir() const -> glm::vec3;
    auto localLeftDir() const -> glm::vec3;

    auto worldRightDir() const -> glm::vec3;
    auto localRightDir() const -> glm::vec3;

    auto worldForwardDir() const -> glm::vec3;
    auto localForwardDir() const -> glm::vec3;

    auto worldBackDir() const -> glm::vec3;
    auto localBackDir() const -> glm::vec3;

    auto translateLocal(const glm::vec3 &translation) -> Transform &;
    auto scaleLocal(const glm::vec3 &scale) -> Transform &;

    auto setLocalPosition(const glm::vec3 &position) -> Transform &;
    auto setLocalScale(const glm::vec3 &scale) -> Transform &;

    auto rotate(const glm::quat &rotation, TransformSpace space = TransformSpace::Self) -> Transform &;
    auto rotate(const glm::vec3 &axis, float angle, TransformSpace space = TransformSpace::Self) -> Transform &;

    auto setLocalRotation(const glm::quat &rotation) -> Transform &;
    auto setLocalRotation(const glm::vec3 &axis, float angle) -> Transform &;

    auto lookAt(const glm::vec3 &target, const glm::vec3 &up) -> Transform &;

    auto matrix() const -> glm::mat4;
    auto worldMatrix() const -> glm::mat4;
    auto invTransposedWorldMatrix() const -> glm::mat4;
    auto worldViewMatrix(const Camera &camera) const -> glm::mat4;
    auto worldViewProjMatrix(const Camera &camera) const -> glm::mat4;
    auto invTransposedWorldViewMatrix(const Camera &camera) const -> glm::mat4;

    auto transformPoint(const glm::vec3 &point) const -> glm::vec3;
    auto transformDirection(const glm::vec3 &direction) const -> glm::vec3;

private:
    uint32_t version_ = 0;
    mutable uint32_t dirtyFlags_ = ~0;

    Transform *parent_ = nullptr;
    std::vector<Transform *> children_;

    glm::vec3 localPosition_;
    glm::vec3 localScale_{1, 1, 1};
    glm::quat localRotation_;
    mutable glm::mat4 matrix_;
    mutable glm::mat4 worldMatrix_;
    mutable glm::mat4 invTransposedWorldMatrix_;

    void setDirtyWithChildren(uint32_t flags);
    void setChildrenDirty(uint32_t flags);
};
