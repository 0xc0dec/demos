/**
 * Copyright (c) Aleksey Fedotov
 * MIT licence
 */

#pragma once

class Window;
class Transform;

void applySpectator(Transform &transform, Window &window, float mouseSensitivity = 0.002f, float movementSpeed = 10);
