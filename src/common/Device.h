/*
    Copyright (c) Aleksey Fedotov
    MIT license
*/

#pragma once

#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <glm/vec2.hpp>
#include <unordered_map>
#include <unordered_set>

class Device
{
public:
    Device(uint32_t canvasWidth, uint32_t canvasHeight, const char *title, bool fullScreen);
    Device(const Device &other) = delete;
    Device(Device &&other) = delete;
    ~Device();

    auto operator=(const Device &other) -> Device& = delete;
    auto operator=(Device &&other) -> Device& = delete;

    void beginUpdate();
    void endUpdate();

	void setCursorCaptured(bool captured);

    bool isKeyPressed(SDL_Keycode code, bool firstTime = false) const;
    bool isKeyReleased(SDL_Keycode code) const;

    auto getMouseMotion() const -> glm::vec2;
    bool isMouseButtonDown(uint8_t button, bool firstTime = false) const;
    bool isMouseButtonReleased(uint8_t button) const;

    bool closeRequested() const { return _closeRequested; }

    auto getTimeDelta() const -> float { return dt; }

private:
    SDL_Window* window = nullptr;
    SDL_GLContext context = nullptr;
    
    float dt = 0;
    bool _closeRequested = false;

	bool hasMouseFocus = false;
    bool hasKeyboardFocus = false;
    bool firstUpdate = true;

    std::unordered_map<SDL_Keycode, bool> pressedKeys;
    std::unordered_set<SDL_Keycode> releasedKeys;

    int32_t mouseDeltaX = 0;
    int32_t mouseDeltaY = 0;
    std::unordered_map<uint8_t, bool> pressedMouseButtons;
    std::unordered_set<uint8_t> releasedMouseButtons;

    void prepareMouseState();
    void prepareKeyboardState();
    void readWindowState();
    void processKeyboardEvent(const SDL_Event &evt);
    void processMouseEvent(const SDL_Event &evt);
};

