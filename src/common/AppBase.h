/*
    Copyright (c) Aleksey Fedotov
    MIT license
*/

#pragma once

#include "Device.h"
#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <GL/glew.h>
#include <vector>

class AppBase
{
public:
	AppBase(uint32_t canvasWidth, uint32_t canvasHeight, bool fullScreen);

	virtual ~AppBase() = default;

	void run();

protected:
	uint32_t canvasWidth_ = 0;
	uint32_t canvasHeight_ = 0;
	Device device_;

	virtual void init() {}
	virtual void render() {}
	virtual void cleanup() {}

    static auto readFile(const char* path) -> std::vector<uint8_t>;
	static auto dataPath(const char *path) -> std::string;
};