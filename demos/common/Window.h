/**
 * Copyright (c) Aleksey Fedotov
 * MIT licence
 */

#pragma once

#define SDL_MAIN_HANDLED
#include <functional>
#include <SDL.h>
#include <glm/vec2.hpp>
#include <unordered_map>
#include <unordered_set>

class Window
{
public:
    Window(uint32_t canvasWidth, uint32_t canvasHeight);
    Window(const Window &other) = delete;
    Window(Window &&other) = delete;

    virtual ~Window();

    auto operator=(const Window &other) -> Window & = delete;
    auto operator=(Window &&other) -> Window & = delete;

    virtual void beginUpdate();
    virtual void endUpdate() {}

    void setCursorCaptured(bool captured);

    bool isKeyPressed(SDL_Keycode code, bool firstTime = false) const;
    bool isKeyReleased(SDL_Keycode code) const;

    auto mouseMotion() const -> glm::vec2;
    bool isMouseButtonDown(uint8_t button, bool firstTime = false) const;
    bool isMouseButtonReleased(uint8_t button) const;

    auto canvasWidth() const -> uint32_t { return canvasWidth_; }
    auto canvasHeight() const -> uint32_t { return canvasHeight_; }

    bool closeRequested() const { return closeRequested_; }

    auto timeDelta() const -> float { return dt_; }

    auto sdlWindow() const -> SDL_Window * { return window_; }

    void onProcessEvent(const std::function<void(SDL_Event &)> &handler) { eventHandler_ = handler; }

protected:
    SDL_Window *window_ = nullptr;

private:
    float dt_ = 0;
    bool closeRequested_ = false;

    bool hasMouseFocus_ = false;
    bool hasKeyboardFocus_ = false;
    bool firstUpdate_ = true;

    uint32_t canvasWidth_{};
    uint32_t canvasHeight_{};

    std::unordered_map<SDL_Keycode, bool> pressedKeys_;
    std::unordered_set<SDL_Keycode> releasedKeys_;

    int32_t mouseDeltaX_ = 0;
    int32_t mouseDeltaY_ = 0;
    std::unordered_map<uint8_t, bool> pressedMouseButtons_;
    std::unordered_set<uint8_t> releasedMouseButtons_;

    std::function<void(SDL_Event &)> eventHandler_;

    void prepareMouseState();
    void prepareKeyboardState();
    void readWindowState();
    void processKeyboardEvent(const SDL_Event &evt);
    void processMouseEvent(const SDL_Event &evt);
};
