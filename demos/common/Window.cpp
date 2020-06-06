/*
    Copyright (c) Aleksey Fedotov
    MIT license
*/

#include "Window.h"
#include "Common.h"

Window::Window(uint32_t canvasWidth, uint32_t canvasHeight):
	canvasWidth_(canvasWidth),
	canvasHeight_(canvasHeight)
{
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_EVENTS) < 0)
		panic("Failed to initialize SDL");
}

Window::~Window()
{
	if (window_)
        SDL_DestroyWindow(window_);
    SDL_Quit();
}

void Window::beginUpdate()
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

    	if (eventHandler_)
			eventHandler_(evt);

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

void Window::setCursorCaptured(bool captured)
{
    SDL_SetRelativeMouseMode(captured ? SDL_TRUE : SDL_FALSE);
}

void Window::prepareMouseState()
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

void Window::prepareKeyboardState()
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

void Window::readWindowState()
{
    const auto flags = SDL_GetWindowFlags(window_);
    hasKeyboardFocus_ = (flags & SDL_WINDOW_INPUT_FOCUS) != 0;
    hasMouseFocus_ = (flags & SDL_WINDOW_MOUSE_FOCUS) != 0;
}

void Window::processKeyboardEvent(const SDL_Event &evt)
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

void Window::processMouseEvent(const SDL_Event &evt)
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

bool Window::isKeyPressed(SDL_Keycode code, bool firstTime) const
{
    const auto where = pressedKeys_.find(code);
    return where != pressedKeys_.end() && (!firstTime || where->second);
}

bool Window::isKeyReleased(SDL_Keycode code) const
{
    return releasedKeys_.find(code) != releasedKeys_.end();
}

auto Window::mouseMotion() const -> glm::vec2
{
    return {mouseDeltaX_, mouseDeltaY_};
}

bool Window::isMouseButtonDown(uint8_t button, bool firstTime) const
{
    const auto where = pressedMouseButtons_.find(button);
    return where != pressedMouseButtons_.end() && (!firstTime || where->second);
}

bool Window::isMouseButtonReleased(uint8_t button) const
{
    return releasedMouseButtons_.find(button) != releasedMouseButtons_.end();
}