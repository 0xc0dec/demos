/*
    Copyright (c) Aleksey Fedotov
    MIT license
*/

#pragma once

#include <memory>
#include <iostream>

#define PANIC(msg) do { std::cerr << msg << std::endl; exit(1); } while (0)

#if defined(WIN32) || defined(_WIN32) || defined(_WIN64) || defined(_WINDOWS)
#   define WINDOWS_APP
#elif defined(__APPLE__) || defined(__MACH__)
#   define OSX_APP
#elif defined(__linux__)
#   define LINUX_APP
#endif