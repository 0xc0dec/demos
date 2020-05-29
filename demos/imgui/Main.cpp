/*
    Copyright (c) Aleksey Fedotov
    MIT license
*/

// This demo includes *.cpp files from ImGui in order to compile, this is apparently the way recommended by ImGui.
// ImGui include paths are configured in CMake.

#include "common/GLAppBase.h"
#include <imgui.h>
// Include these two as we use SDL + OpenGL stack
#include <examples/imgui_impl_opengl3.h>
#include <examples/imgui_impl_sdl.h>
#include <GL/glew.h>

class App final : public GLAppBase
{
public:
	App(): GLAppBase(1366, 768, false)
	{
	}

private:
	void init() override
	{
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;

		ImGui_ImplSDL2_InitForOpenGL(device()->sdlWindow(), device()->sdlGLContext());
		ImGui_ImplOpenGL3_Init("#version 130"); // GL 3.0 + GLSL 130

		device()->onProcessEvent([](auto &evt)
		{
			ImGui_ImplSDL2_ProcessEvent(&evt);
		});
	}

	void render() override
	{
		// "Logical" render of ImGUI
		
		ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame(device()->sdlWindow());
        ImGui::NewFrame();

		auto open = true; // never close
		ImGui::ShowDemoWindow(&open);

		ImGui::Render();

		// The actual render via OpenGL
		
		glViewport(0, 0, device()->canvasWidth(), device()->canvasHeight());
		glClearColor(0, 0.5f, 0.6f, 1);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}

	void cleanup() override
	{
		ImGui_ImplOpenGL3_Shutdown();
	    ImGui_ImplSDL2_Shutdown();
	    ImGui::DestroyContext();
	}
};

int main()
{
	App().run();
	return 0;
}
