/**
 * Copyright (c) Aleksey Fedotov
 * MIT licence
 */

#pragma once

#include "Window.h"
#include <memory>
#include <vector>

class AppBase
{
public:
    virtual ~AppBase() = default;

    void run();

protected:
    std::unique_ptr<Window> window_;

    explicit AppBase(std::unique_ptr<Window> window);

    virtual void init() = 0;
    virtual void render() = 0;
    virtual void cleanup() = 0;

    static auto readFile(const char *path) -> std::vector<uint8_t>;
    static auto assetPath(const char *path) -> std::string;
};
