/* 
 * Copyright (c) Aleksey Fedotov 
 * MIT license 
 */

#pragma once

#include <string>
#include <vector>

namespace vk
{
    enum class VertexAttributeUsage
    {
        Unknown,
        Position,
        Normal,
        TexCoord,
        Tangent,
        Binormal
    };

    class VertexAttribute final
    {
    public:
        std::string name;
        uint32_t elementCount;
        uint32_t size;
        uint32_t offset;
        VertexAttributeUsage usage;
    };

    class VertexBufferLayout final
    {
    public:
        void addAttribute(VertexAttributeUsage usage);

        auto attributeCount() const -> uint32_t { return static_cast<uint32_t>(attributes_.size()); }
        auto attribute(uint32_t index) const -> VertexAttribute { return attributes_.at(index); }
    	auto attributeIndex(VertexAttributeUsage usage) const -> int;

        auto size() const -> uint32_t { return size_; }
        auto elementCount() const -> uint32_t { return elementCount_; }

    private:
        std::vector<VertexAttribute> attributes_;
        uint32_t size_ = 0; // in bytes
        uint32_t elementCount_ = 0; // number of "elements" (floats)

        void addAttribute(uint32_t elementCount, const std::string &name, VertexAttributeUsage usage);
    };
}
