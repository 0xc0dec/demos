#pragma once

namespace SkyboxDemo
{
    struct Shaders
    {
        struct
        {
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