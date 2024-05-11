/**
 * Copyright (c) Aleksey Fedotov
 * MIT licence
 */

#include "VertexBufferLayout.h"
#include "Common.h"

void vk::VertexBufferLayout::addAttribute(uint32_t elementCount, const std::string &name, VertexAttributeUsage usage)
{
    const auto size = static_cast<uint32_t>(sizeof(float) * elementCount);
    const auto offset = attributes_.empty() ? 0 : attributes_.crbegin()->offset + attributes_.crbegin()->size;
    attributes_.push_back(VertexAttribute{name, elementCount, size, offset, usage});
    this->elementCount_ += elementCount;
    this->size_ += size;
}

void vk::VertexBufferLayout::addAttribute(VertexAttributeUsage usage)
{
    switch (usage)
    {
    case VertexAttributeUsage::Position:
        addAttribute(3, "sl_Position", VertexAttributeUsage::Position);
        break;
    case VertexAttributeUsage::Normal:
        addAttribute(3, "sl_Normal", VertexAttributeUsage::Normal);
        break;
    case VertexAttributeUsage::TexCoord:
        addAttribute(2, "sl_TexCoord", VertexAttributeUsage::TexCoord);
        break;
    case VertexAttributeUsage::Tangent:
        addAttribute(3, "sl_Tangent", VertexAttributeUsage::Tangent);
        break;
    case VertexAttributeUsage::Binormal:
        addAttribute(3, "sl_Binormal", VertexAttributeUsage::Binormal);
        break;
    default:
        panic("Unsupported vertex attribute usage");
    }
}

auto vk::VertexBufferLayout::attributeIndex(VertexAttributeUsage usage) const -> int
{
    for (int i = 0; i < attributes_.size(); i++)
    {
        if (attributes_[i].usage == usage)
            return i;
    }
    return -1;
}
