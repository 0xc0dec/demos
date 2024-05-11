/**
 * Copyright (c) Aleksey Fedotov
 * MIT licence
 */

#pragma once

#include <iostream>

#if defined(WIN32) || defined(_WIN32) || defined(_WIN64) || defined(_WINDOWS)
#define WINDOWS_APP
#elif defined(__APPLE__) || defined(__MACH__)
#define OSX_APP
#elif defined(__linux__)
#define LINUX_APP
#endif

template <class TArg>
constexpr void panic(TArg &&arg)
{
    std::cerr << arg << std::endl;
    exit(1);
}

template <class TFirst, class... TRest>
constexpr void panic(TFirst &&first, TRest &&...rest)
{
    std::cerr << first;
    panic(std::forward<TRest>(rest)...);
}

template <class... TArgs>
constexpr void panicIf(bool condition, TArgs &&...args)
{
    if (condition)
        panic(std::forward<TArgs>(args)...);
}
