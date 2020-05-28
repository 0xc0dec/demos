/*
    Copyright (c) Aleksey Fedotov
    MIT license
*/

#pragma once

class Device;
class Transform;

void applySpectator(Transform &transform, Device &device, float mouseSensitivity = 0.002f, float movementSpeed = 10);
