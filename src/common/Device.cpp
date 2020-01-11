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
        PANIC("Failed to initialize SDL");

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    const auto x = SDL_WINDOWPOS_CENTERED;
    const auto y = SDL_WINDOWPOS_CENTERED;
    auto flags = SDL_WINDOW_OPENGL | SDL_WINDOW_ALLOW_HIGHDPI;
    if (fullScreen)
        flags |= SDL_WINDOW_FULLSCREEN;

    window_ = SDL_CreateWindow(title, x, y, canvasWidth, canvasHeight, flags);
    context_ = SDL_GL_CreateContext(window_);

    glewExperimental = true;
    glewInit();

    SDL_GL_SetSwapInterval(1);
}

Device::~Device()
{
    if (context_)
        SDL_GL_DeleteContext(context_);
    if (window_)
        SDL_DestroyWindow(window_);
    SDL_Quit();
}

void Device::beginUpdate()
{
    readWindowState();
    prepareMouseState();
    prepareKeyboardState();
    firstUpdate_ = false;

    SDL_Event evt;
    while (SDL_PollEvent(&evt))
    {
		if (!firstUpdate_)
		{
			processKeyboardEvent(evt);
			processMouseEvent(evt);
		}

		const auto closeWindowEvent = evt.type == SDL_WINDOWEVENT && evt.window.event == SDL_WINDOWEVENT_CLOSE;
        if (evt.type == SDL_QUIT || closeWindowEvent)
            closeRequested_ = true;
    }

	static auto lastTicks = SDL_GetTicks();
    const auto ticks = SDL_GetTicks();
	const auto deltaTicks = ticks - lastTicks;
    if (deltaTicks > 0)
    {
        dt_ = deltaTicks / 1000.0f;
        lastTicks = ticks;
    }
}

void Device::endUpdate()
{
	SDL_GL_SwapWindow(window_);
}

void Device::setCursorCaptured(bool captured)
{
    SDL_SetRelativeMouseMode(captured ? SDL_TRUE : SDL_FALSE);
}

void Device::prepareMouseState()
{
    mouseDeltaX_ = mouseDeltaY_ = 0;
    releasedMouseButtons_.clear();
    if (hasMouseFocus_)
    {
        for (const auto &pair : pressedMouseButtons_)
            pressedMouseButtons_[pair.first] = false;
    }
    else
    {
        for (const auto &pair : pressedMouseButtons_)
            releasedMouseButtons_.insert(pair.first);
        pressedMouseButtons_.clear();
    }
}

void Device::prepareKeyboardState()
{
    releasedKeys_.clear();
    if (hasKeyboardFocus_)
    {
        for (auto &pair : pressedKeys_)
            pair.second = false; // not "pressed for the first time" anymore
    }
    else
    {
        for (const auto &pair : pressedKeys_)
            releasedKeys_.insert(pair.first);
        pressedKeys_.clear();
    }
}

void Device::readWindowState()
{
    const auto flags = SDL_GetWindowFlags(window_);
    hasKeyboardFocus_ = (flags & SDL_WINDOW_INPUT_FOCUS) != 0;
    hasMouseFocus_ = (flags & SDL_WINDOW_MOUSE_FOCUS) != 0;
}

void Device::processKeyboardEvent(const SDL_Event &evt)
{
    if (!hasKeyboardFocus_)
        return;

    switch (evt.type)
    {
        case SDL_KEYUP:
        case SDL_KEYDOWN:
        {
            const auto code = evt.key.keysym.sym;
            if (evt.type == SDL_KEYUP)
            {
                releasedKeys_.insert(code);
                pressedKeys_.erase(code);
            }
            else
            {
                pressedKeys_[code] = pressedKeys_.find(code) == pressedKeys_.end(); // first time?
                releasedKeys_.erase(code);
            }
            break;
        }
        default:
            break;
    }
}

void Device::processMouseEvent(const SDL_Event &evt)
{
    if (!hasMouseFocus_)
        return;

    switch (evt.type)
    {
        case SDL_MOUSEMOTION:
            mouseDeltaX_ += evt.motion.xrel;
            mouseDeltaY_ += evt.motion.yrel;
            break;
        case SDL_MOUSEBUTTONDOWN:
        {
            const auto btn = evt.button.button;
            pressedMouseButtons_[btn] = true; // pressed for the first time
            releasedMouseButtons_.erase(btn);
            break;
        }
        case SDL_MOUSEBUTTONUP:
        {
            const auto btn = evt.button.button;
            if (pressedMouseButtons_.find(btn) != pressedMouseButtons_.end())
            {
                releasedMouseButtons_.insert(btn);
                pressedMouseButtons_.erase(btn);
            }
            break;
        }
        default:
            break;
    }
}

bool Device::isKeyPressed(SDL_Keycode code, bool firstTime) const
{
    const auto where = pressedKeys_.find(code);
    return where != pressedKeys_.end() && (!firstTime || where->second);
}

bool Device::isKeyReleased(SDL_Keycode code) const
{
    return releasedKeys_.find(code) != releasedKeys_.end();
}

auto Device::mouseMotion() const -> glm::vec2
{
    return {mouseDeltaX_, mouseDeltaY_};
}

bool Device::isMouseButtonDown(uint8_t button, bool firstTime) const
{
    const auto where = pressedMouseButtons_.find(button);
    return where != pressedMouseButtons_.end() && (!firstTime || where->second);
}

bool Device::isMouseButtonReleased(uint8_t button) const
{
    return releasedMouseButtons_.find(button) != releasedMouseButtons_.end();
}