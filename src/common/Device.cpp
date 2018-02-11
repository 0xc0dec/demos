/*
    Copyright (c) Aleksey Fedotov
    MIT license
*/

#include "Device.h"
#include "Common.h"
#include <GL/glew.h>
#include <iostream>
#include <vector>

Device::Device(uint32_t canvasWidth, uint32_t canvasHeight, const char *title, bool fullScreen)
{
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_EVENTS) < 0)
        DIE("Failed to initialize SDL");

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    const auto x = SDL_WINDOWPOS_CENTERED;
    const auto y = SDL_WINDOWPOS_CENTERED;
    auto flags = SDL_WINDOW_OPENGL | SDL_WINDOW_ALLOW_HIGHDPI;
    if (fullScreen)
        flags |= SDL_WINDOW_FULLSCREEN;

    window = SDL_CreateWindow(title, x, y, canvasWidth, canvasHeight, flags);
    context = SDL_GL_CreateContext(window);

    glewExperimental = true;
    glewInit();

    SDL_GL_SetSwapInterval(1);
}

Device::~Device()
{
    if (context)
        SDL_GL_DeleteContext(context);
    if (window)
        SDL_DestroyWindow(window);
    SDL_Quit();
}

void Device::beginUpdate()
{
    readWindowState();
    prepareMouseState();
    prepareKeyboardState();
    firstUpdate = false;

    SDL_Event evt;
    while (SDL_PollEvent(&evt))
    {
		if (!firstUpdate)
		{
			processKeyboardEvent(evt);
			processMouseEvent(evt);
		}

		const auto closeWindowEvent = evt.type == SDL_WINDOWEVENT && evt.window.event == SDL_WINDOWEVENT_CLOSE;
        if (evt.type == SDL_QUIT || closeWindowEvent)
            _closeRequested = true;
    }

	static auto lastTicks = SDL_GetTicks();
    const auto ticks = SDL_GetTicks();
	const auto deltaTicks = ticks - lastTicks;
    if (deltaTicks > 0)
    {
        dt = deltaTicks / 1000.0f;
        lastTicks = ticks;
    }
}

void Device::endUpdate()
{
	SDL_GL_SwapWindow(window);
}

void Device::setCursorCaptured(bool captured)
{
    SDL_SetRelativeMouseMode(captured ? SDL_TRUE : SDL_FALSE);
}

void Device::prepareMouseState()
{
    mouseDeltaX = mouseDeltaY = 0;
    releasedMouseButtons.clear();
    if (hasMouseFocus)
    {
        for (const auto &pair : pressedMouseButtons)
            pressedMouseButtons[pair.first] = false;
    }
    else
    {
        for (const auto &pair : pressedMouseButtons)
            releasedMouseButtons.insert(pair.first);
        pressedMouseButtons.clear();
    }
}

void Device::prepareKeyboardState()
{
    releasedKeys.clear();
    if (hasKeyboardFocus)
    {
        for (auto &pair : pressedKeys)
            pair.second = false; // not "pressed for the first time" anymore
    }
    else
    {
        for (const auto &pair : pressedKeys)
            releasedKeys.insert(pair.first);
        pressedKeys.clear();
    }
}

void Device::readWindowState()
{
    const auto flags = SDL_GetWindowFlags(window);
    hasKeyboardFocus = (flags & SDL_WINDOW_INPUT_FOCUS) != 0;
    hasMouseFocus = (flags & SDL_WINDOW_MOUSE_FOCUS) != 0;
}

void Device::processKeyboardEvent(const SDL_Event &evt)
{
    if (!hasKeyboardFocus)
        return;

    switch (evt.type)
    {
        case SDL_KEYUP:
        case SDL_KEYDOWN:
        {
            const auto code = evt.key.keysym.sym;
            if (evt.type == SDL_KEYUP)
            {
                releasedKeys.insert(code);
                pressedKeys.erase(code);
            }
            else
            {
                pressedKeys[code] = pressedKeys.find(code) == pressedKeys.end(); // first time?
                releasedKeys.erase(code);
            }
            break;
        }
        default:
            break;
    }
}

void Device::processMouseEvent(const SDL_Event &evt)
{
    if (!hasMouseFocus)
        return;

    switch (evt.type)
    {
        case SDL_MOUSEMOTION:
            mouseDeltaX += evt.motion.xrel;
            mouseDeltaY += evt.motion.yrel;
            break;
        case SDL_MOUSEBUTTONDOWN:
        {
            const auto btn = evt.button.button;
            pressedMouseButtons[btn] = true; // pressed for the first time
            releasedMouseButtons.erase(btn);
            break;
        }
        case SDL_MOUSEBUTTONUP:
        {
            const auto btn = evt.button.button;
            if (pressedMouseButtons.find(btn) != pressedMouseButtons.end())
            {
                releasedMouseButtons.insert(btn);
                pressedMouseButtons.erase(btn);
            }
            break;
        }
        default:
            break;
    }
}

bool Device::isKeyPressed(SDL_Keycode code, bool firstTime) const
{
    const auto where = pressedKeys.find(code);
    return where != pressedKeys.end() && (!firstTime || where->second);
}

bool Device::isKeyReleased(SDL_Keycode code) const
{
    return releasedKeys.find(code) != releasedKeys.end();
}

auto Device::getMouseMotion() const -> glm::vec2
{
    return {mouseDeltaX, mouseDeltaY};
}

bool Device::isMouseButtonDown(uint8_t button, bool firstTime) const
{
    const auto where = pressedMouseButtons.find(button);
    return where != pressedMouseButtons.end() && (!firstTime || where->second);
}

bool Device::isMouseButtonReleased(uint8_t button) const
{
    return releasedMouseButtons.find(button) != releasedMouseButtons.end();
}