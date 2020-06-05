/*
    Copyright (c) Aleksey Fedotov
    MIT license
*/

#include "AppBase.h"
#include "Common.h"
#include "Device.h"
#include <fstream>
#include <string>

void AppBase::run()
{
    init();

    while (!device_->closeRequested() && !device_->isKeyPressed(SDLK_ESCAPE, true))
    {
        device_->beginUpdate();
        render();
        device_->endUpdate();
    }

    cleanup();
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
