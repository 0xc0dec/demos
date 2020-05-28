/*
    Copyright (c) Aleksey Fedotov
    MIT license
*/

#pragma once

#include "Device.h"
#include <vector>

class AppBase
{
public:
	AppBase(uint32_t canvasWidth, uint32_t canvasHeight, bool fullScreen);

	virtual ~AppBase() = default;

	void run();

protected:
	auto canvasWidth() const -> uint32_t { return canvasWidth_; }
	auto canvasHeight() const -> uint32_t { return canvasHeight_; }
	auto device() -> Device& { return device_; }

	virtual void init() = 0;
	virtual void render() = 0;
	virtual void cleanup() = 0;

    static auto readFile(const char* path) -> std::vector<uint8_t>;
	static auto assetPath(const char *path) -> std::string;

private:
	uint32_t canvasWidth_ = 0;
	uint32_t canvasHeight_ = 0;
	Device device_;
};