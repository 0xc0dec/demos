/*
    Copyright (c) Aleksey Fedotov
    MIT license
*/

#include "common/vk/VulkanAppBase.h"

class App: public vk::AppBase
{
public:
	App(): vk::AppBase(1366, 768, false)
	{
	}

protected:
	void init() override
	{
	}
	
	void render() override
	{
		
	}
	
	void cleanup() override
	{
		
	}
};

void main()
{
	App().run();
}