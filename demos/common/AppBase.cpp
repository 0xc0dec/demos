/**
 * Copyright (c) Aleksey Fedotov
 * MIT licence
 */

#include "AppBase.h"
#include "Common.h"
#include "Window.h"
#include <fstream>
#include <string>

void AppBase::run()
{
    init();

    while (!window_->closeRequested() && !window_->isKeyPressed(SDLK_ESCAPE, true))
    {
        window_->beginUpdate();
        render();
        window_->endUpdate();
    }

    cleanup();
}

AppBase::AppBase(std::unique_ptr<Window> window) : window_(std::move(window))
{
}

auto AppBase::readFile(const char *path) -> std::vector<uint8_t>
{
    std::ifstream file(path, std::ios::binary | std::ios::ate);
    if (!file.is_open())
        panic("Failed to open file ", path);

    const auto size = file.tellg();
    file.seekg(0, std::ios::beg);
    auto bytes = std::vector<uint8_t>(size);
    file.read(reinterpret_cast<char *>(&bytes[0]), size);
    file.close();

    return bytes;
}

auto AppBase::assetPath(const char *path) -> std::string
{
    return std::string("../../../assets/") + path;
}
