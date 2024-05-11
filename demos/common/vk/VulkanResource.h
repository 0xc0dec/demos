/**
 * Copyright (c) Aleksey Fedotov
 * MIT licence
 */

#pragma once

#include "../Common.h"

#ifdef WINDOWS_APP
#define VK_USE_PLATFORM_WIN32_KHR
#endif
#include <vulkan.h>
#include <functional>
#include <cassert>

namespace vk
{
    template <class T>
    class Resource
    {
    public:
        Resource()
        {
        }

        Resource(const Resource<T> &other) = delete;

        Resource(Resource<T> &&other) noexcept
        {
            swap(other);
        }

        explicit Resource(std::function<void(T, VkAllocationCallbacks *)> del)
        {
            this->del_ = [=](T handle)
            { del(handle, nullptr); };
        }

        Resource(VkInstance instance, std::function<void(VkInstance, T, VkAllocationCallbacks *)> del)
        {
            this->del_ = [instance, del](T obj)
            { del(instance, obj, nullptr); };
        }

        Resource(VkDevice device, std::function<void(VkDevice, T, VkAllocationCallbacks *)> del)
        {
            this->del_ = [device, del](T obj)
            { del(device, obj, nullptr); };
        }

        Resource(VkDevice device, VkCommandPool cmdPool, std::function<void(VkDevice, VkCommandPool, uint32_t, T *)> del)
        {
            this->del_ = [device, cmdPool, del](T obj)
            { del(device, cmdPool, 1, &obj); };
        }

        ~Resource()
        {
            cleanup();
        }

        auto operator=(Resource<T> other) noexcept -> Resource<T> &
        {
            swap(other);
            return *this;
        }

        auto operator&() const -> const T * { return &handle_; }
        auto operator&() -> T * { return &handle_; }

        auto cleanRef() -> T *
        {
            ensureInitialized();
            cleanup();
            return &handle_;
        }

        operator T() const { return handle_; }
        operator bool() const { return handle_ != VK_NULL_HANDLE; }

        template <typename V>
        bool operator==(V rhs) { return handle_ == T(rhs); }

    private:
        T handle_ = VK_NULL_HANDLE;
        std::function<void(T)> del_;

        void cleanup()
        {
            if (handle_ != VK_NULL_HANDLE)
            {
                ensureInitialized();
                del_(handle_);
            }
            handle_ = VK_NULL_HANDLE;
        }

        void ensureInitialized()
        {
            assert(del_ /* Calling cleanup() on a Resource with empty deleter */);
        }

        void swap(Resource<T> &other) noexcept
        {
            std::swap(handle_, other.handle_);
            std::swap(del_, other.del_);
        }
    };
}
