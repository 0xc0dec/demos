/**
 * Copyright (c) Aleksey Fedotov
 * MIT licence
 */

#pragma once

struct Shaders
{
    struct
    {
        const char *simple =
            R"(
                #version 330 core

                in vec4 position;
                in vec2 texCoord0;

                uniform mat4 worldMatrix;
                uniform mat4 viewProjMatrix;
                out vec2 uv0;

                void main()
                {
                    gl_Position = viewProjMatrix * worldMatrix * position;
                    uv0 = texCoord0;
                }
            )";
    } vertex;

    struct
    {
        const char *simple =
            R"(
                #version 330 core

                in vec2 uv0;
                out vec4 fragColor;

                void main()
                {
                    fragColor = vec4(uv0.x, uv0.y, 0, 1);
                }
            )";
    } fragment;
};
