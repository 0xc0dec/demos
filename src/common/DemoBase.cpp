/*
    Copyright (c) Aleksey Fedotov
    MIT license
*/

#include "DemoBase.h"
#include "Common.h"
#include "Device.h"
#include <GL/glew.h>
#include <fstream>
#include <unordered_map>
#include <string>

DemoBase::DemoBase(uint32_t canvasWidth, uint32_t canvasHeight, bool fullScreen):
	canvasWidth(canvasWidth),
	canvasHeight(canvasHeight),
	device(canvasWidth, canvasHeight, "Demo", fullScreen)
{
}

void DemoBase::run()
{
	init();
	while (!device.closeRequested() && !device.isKeyPressed(SDLK_ESCAPE, true))
	{
		device.beginUpdate();
		render();
		device.endUpdate();
	}
	shutdown();
}

auto DemoBase::createProgram(const char* vs, const char* fs) -> GLuint
{
    const auto vertex = compileShader(GL_VERTEX_SHADER, vs);
    const auto fragment = compileShader(GL_FRAGMENT_SHADER, fs);
    const auto program = linkProgram(vertex, fragment);
    glDeleteShader(vertex);
    glDeleteShader(fragment);
    return program;
}

auto DemoBase::readFile(const char* path) -> std::vector<uint8_t>
{
    std::ifstream file(path, std::ios::binary | std::ios::ate);
    if (!file.is_open())
        PANIC("Failed to open file " << path);
    
	const auto size = file.tellg();
    file.seekg(0, std::ios::beg);
    auto bytes = std::vector<uint8_t>(size);
    file.read(reinterpret_cast<char*>(&bytes[0]), size);
    file.close();
    
	return bytes;
}

auto DemoBase::compileShader(GLenum type, const char* src) -> GLuint
{
    static std::unordered_map<GLuint, std::string> typeNames =
    {
        { GL_VERTEX_SHADER, "vertex" },
        { GL_FRAGMENT_SHADER, "fragment" }
    };

    const auto shader = glCreateShader(type);

    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);

    GLint status;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
    if (status == GL_FALSE)
    {
        GLint logLength;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);
        std::vector<GLchar> log(logLength);
        glGetShaderInfoLog(shader, logLength, nullptr, log.data());
        glDeleteShader(shader);
        PANIC("Failed to compile " << typeNames[type] << " shader: " << log.data());
    }

    return shader;
}

auto DemoBase::linkProgram(GLuint vs, GLuint fs) -> GLint
{
    const auto program = glCreateProgram();
    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);

    GLint status;
    glGetProgramiv(program, GL_LINK_STATUS, &status);
    if (status == GL_FALSE)
    {
        GLint logLength;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLength);
        std::vector<GLchar> log(logLength);
        glGetProgramInfoLog(program, logLength, nullptr, log.data());
        glDeleteProgram(program);
        PANIC("Failed to link program: " << log.data());
    }

    return program;
}