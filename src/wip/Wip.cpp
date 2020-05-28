/*
    Copyright (c) Aleksey Fedotov
    MIT license
*/

#include "common/AppBase.h"
#include <imgui.h>
#include <examples/imgui_impl_opengl3.h>

class App final : public AppBase
{
public:
	App(): AppBase(1366, 768, false)
	{
	}

private:
	void init() override
	{
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
	}

	void render() override
	{
	}
};

int main()
{
	App().run();
	return 0;
}
