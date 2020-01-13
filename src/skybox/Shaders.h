#pragma once

namespace SkyboxDemo
{
    struct Shaders
    {
        struct
        {
            const char *simple = R"(
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

            const char *skybox = R"(
                #version 330 core

                in vec4 position;

                uniform mat4 viewMatrix;
                uniform mat4 projMatrix;
                out vec3 eyeDir;

                void main()
                {
                    mat4 invProjMatrix = inverse(projMatrix);
                    mat3 invViewMatrix = inverse(mat3(viewMatrix));
                    vec3 unprojected = (invProjMatrix * position).xyz;
                    eyeDir = invViewMatrix * unprojected;

                    gl_Position = position;
                }
            )";
        } vertex;

        struct
        {
            const char *simple = R"(
                #version 330 core

                in vec2 uv0;
                out vec4 fragColor;

                void main()
                {
                    fragColor = vec4(uv0.x, uv0.y, 0, 1);
                }
            )";

            const char *skybox = R"(
                #version 330 core

                uniform samplerCube skyboxTex;

                in vec3 eyeDir;
                out vec4 fragColor;

                void main()
                {
                    fragColor = texture(skyboxTex, eyeDir);
                }               
            )";
        } fragment;
    };
}