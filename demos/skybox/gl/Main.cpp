/**
 * Copyright (c) Aleksey Fedotov
 * MIT licence
 */

#include "common/Camera.h"
#include "common/Spectator.h"
#include "common/gl/OpenGLMesh.h"
#include "common/gl/OpenGLAppBase.h"
#include "common/gl/OpenGLShaderProgram.h"
#include "Shaders.h"
#include <memory>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

class App final : public gl::AppBase
{
public:
    App() : gl::AppBase(1366, 768, false)
    {
    }

private:
    std::shared_ptr<gl::Mesh> quadMesh_;
    std::shared_ptr<gl::Mesh> boxMesh_;

    std::shared_ptr<gl::ShaderProgram> skyboxShader_;
    std::shared_ptr<gl::ShaderProgram> meshShader_;

    struct
    {
        GLuint handle;
    } texture_;

    Camera camera_;
    Transform meshTransform_;

    void init() override
    {
        initShaders();
        initTextures();

        quadMesh_ = gl::Mesh::quad();
        boxMesh_ = gl::Mesh::box();

        camera_.setPerspective(45, 1.0f * window()->canvasWidth() / window()->canvasHeight(), 0.1f, 100.0f);
        camera_.transform().setLocalPosition({10, 10, 10});
        camera_.transform().lookAt({0, 0, 0}, {0, 1, 0});
    }

    void loadFaceData(const char *path, GLenum target) const
    {
        auto data = readFile(path);

        stbi_set_flip_vertically_on_load(true); // for OpenGL

        int width, height, channels;
        const auto handle = stbi_load_from_memory(data.data(), data.size(), &width, &height, &channels, 4);

        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        glTexImage2D(target, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, handle);

        stbi_image_free(handle);
    }

    void initTextures()
    {
        glGenTextures(1, &texture_.handle);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, texture_.handle);

        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAX_LEVEL, 0);

        loadFaceData(assetPath("textures/skyboxes/deep-space/+x.png").c_str(), GL_TEXTURE_CUBE_MAP_POSITIVE_X);
        loadFaceData(assetPath("textures/skyboxes/deep-space/-x.png").c_str(), GL_TEXTURE_CUBE_MAP_NEGATIVE_X);
        loadFaceData(assetPath("textures/skyboxes/deep-space/+y.png").c_str(), GL_TEXTURE_CUBE_MAP_POSITIVE_Y);
        loadFaceData(assetPath("textures/skyboxes/deep-space/-y.png").c_str(), GL_TEXTURE_CUBE_MAP_NEGATIVE_Y);
        loadFaceData(assetPath("textures/skyboxes/deep-space/+z.png").c_str(), GL_TEXTURE_CUBE_MAP_POSITIVE_Z);
        loadFaceData(assetPath("textures/skyboxes/deep-space/-z.png").c_str(), GL_TEXTURE_CUBE_MAP_NEGATIVE_Z);

        glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    }

    void initShaders()
    {
        static Shaders shaders;

        skyboxShader_ = std::make_shared<gl::ShaderProgram>(shaders.vertex.skybox, shaders.fragment.skybox);
        meshShader_ = std::make_shared<gl::ShaderProgram>(shaders.vertex.simple, shaders.fragment.simple);
    }

    void cleanup() override
    {
        glDeleteTextures(1, &texture_.handle);
    }

    void render() override
    {
        applySpectator(camera_.transform(), *window());

        glViewport(0, 0, window()->canvasWidth(), window()->canvasHeight());
        glClear(GL_DEPTH_BUFFER_BIT); // no need to clear color since we're rendering fullscreen quad anyway

        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);

        // Skybox
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, texture_.handle);

        // Don't write to the depth buffer (the quad is always closest to the camera and prevents other objects from showing)
        glDepthMask(GL_FALSE);

        skyboxShader_->use();
        skyboxShader_->setMatrixUniform("projMatrix", glm::value_ptr(camera_.projMatrix()));
        skyboxShader_->setMatrixUniform("viewMatrix", glm::value_ptr(camera_.viewMatrix()));
        skyboxShader_->setTextureUniform("skyboxTex", 0);

        quadMesh_->draw();

        // Mesh

        glDepthMask(GL_TRUE);

        meshShader_->use();
        meshShader_->setMatrixUniform("viewProjMatrix", glm::value_ptr(camera_.viewProjMatrix()));
        meshShader_->setMatrixUniform("worldMatrix", glm::value_ptr(meshTransform_.worldMatrix()));

        boxMesh_->draw();
    }
};

int main()
{
    App().run();
    return 0;
}
