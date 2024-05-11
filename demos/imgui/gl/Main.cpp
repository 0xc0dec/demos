/**
 * Copyright (c) Aleksey Fedotov
 * MIT licence
 */

#include "common/gl/OpenGLAppBase.h"
#include <imgui.h>
#include <examples/imgui_impl_opengl3.h>
#include <examples/imgui_impl_sdl.h>
#include <GL/glew.h>

class App final : public gl::AppBase
{
public:
    App() : gl::AppBase(1366, 768, false)
    {
    }

private:
    void init() override
    {
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO &io = ImGui::GetIO();
        (void)io;

        ImGui_ImplSDL2_InitForOpenGL(window()->sdlWindow(), window()->sdlGLContext());
        ImGui_ImplOpenGL3_Init("#version 130"); // GL 3.0 + GLSL 130

        window()->onProcessEvent([](auto &evt)
                                 { ImGui_ImplSDL2_ProcessEvent(&evt); });
    }

    void render() override
    {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame(window()->sdlWindow());
        ImGui::NewFrame();

        auto open = true; // never close
        ImGui::ShowDemoWindow(&open);

        ImGui::Render();

        glViewport(0, 0, window()->canvasWidth(), window()->canvasHeight());
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
