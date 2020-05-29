/*
    Copyright (c) Aleksey Fedotov
    MIT license
*/

#include "common/vk/VkAppBase.h"

class App: public VkAppBase
{
public:
	App(): VkAppBase(1366, 768, false)
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