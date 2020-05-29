/*
    Copyright (c) Aleksey Fedotov
    MIT license
*/

#pragma once

#include "Device.h"
#include <memory>
#include <vector>

class AppBase
{
public:
	virtual ~AppBase() = default;

	void run();

protected:
	std::unique_ptr<Device> device_;
	
	virtual void init() = 0;
	virtual void render() = 0;
	virtual void cleanup() = 0;

    static auto readFile(const char* path) -> std::vector<uint8_t>;
	static auto assetPath(const char *path) -> std::string;
};
