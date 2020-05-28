/*
    Copyright (c) Aleksey Fedotov
    MIT license
*/

#include "AppBase.h"
#include "Common.h"
#include "Device.h"
#include <fstream>
#include <string>

AppBase::AppBase(uint32_t canvasWidth, uint32_t canvasHeight, bool fullScreen) :
    canvasWidth_(canvasWidth),
    canvasHeight_(canvasHeight),
    device_(canvasWidth, canvasHeight, "Demo", fullScreen)
{
}

void AppBase::run()
{
    init();

    while (!device_.closeRequested() && !device_.isKeyPressed(SDLK_ESCAPE, true))
    {
        device_.beginUpdate();
        render();
        device_.endUpdate();
    }

    cleanup();
}

auto AppBase::readFile(const char *path) -> std::vector<uint8_t>
{
    std::ifstream file(path, std::ios::binary | std::ios::ate);
    if (!file.is_open())
        PANIC("Failed to open file " << path);

    const auto size = file.tellg();
    file.seekg(0, std::ios::beg);
    auto bytes = std::vector<uint8_t>(size);
    file.read(reinterpret_cast<char *>(&bytes[0]), size);
    file.close();

    return bytes;
}

auto AppBase::dataPath(const char *path) -> std::string
{
    return std::string("../../data/") + path;
}
