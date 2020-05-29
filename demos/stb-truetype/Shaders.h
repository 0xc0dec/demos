/*
    Copyright (c) Aleksey Fedotov
    MIT license
*/

#pragma once

struct Shaders
{
	struct
	{
		const char *font =
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
		const char *font =
			R"(
                #version 330 core

                uniform sampler2D mainTex;

                in vec2 uv0;
                out vec4 fragColor;

                void main()
                {
                    vec4 c = texture(mainTex, uv0);
                    fragColor = vec4(c.r, c.r, c.r, c.r);
                }
            )";
	} fragment;
};
