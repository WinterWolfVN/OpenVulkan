#pragma once

#include <cstdint>

using VkBool32 = uint32_t;
using VkFlags = uint32_t;
using VkDeviceSize = uint64_t;

enum VkResult
{
    VK_SUCCESS = 0,
    VK_NOT_READY = 1,
    VK_TIMEOUT = 2,

    VK_ERROR_UNKNOWN = -1,
    VK_ERROR_OUT_OF_HOST_MEMORY = -2,
    VK_ERROR_OUT_OF_DEVICE_MEMORY = -3,
    VK_ERROR_INITIALIZATION_FAILED = -4
};

struct VkExtent2D
{
    uint32_t width;
    uint32_t height;
};

struct VkOffset2D
{
    int32_t x;
    int32_t y;
};

struct VkRect2D
{
    VkOffset2D offset;
    VkExtent2D extent;
};
