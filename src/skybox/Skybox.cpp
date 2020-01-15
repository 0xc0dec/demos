/*
    Copyright (c) Aleksey Fedotov
    MIT license
*/

#include "common/AppBase.h"
#include "common/Device.h"
#include "common/Common.h"
#include "common/Camera.h"
#include "common/Spectator.h"
#include "common/Mesh.h"
#include "common/ShaderProgram.h"
#include "Shaders.h"
#include <memory>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

class App: public AppBase
{
public:
	App(): AppBase(1366, 768, false)
	{
	}

private:
	std::shared_ptr<Mesh> quadMesh;
	std::shared_ptr<Mesh> boxMesh;

	std::shared_ptr<ShaderProgram> skyboxShader;
	std::shared_ptr<ShaderProgram> meshShader;

	struct
	{
		GLuint handle;
	} texture;

	Camera camera;
	Transform meshTransform;

	void init() override final
	{
		initShaders();
		initTextures();

		quadMesh = Mesh::quad();
		boxMesh = Mesh::box();

		camera.setPerspective(45, 1.0f * canvasWidth_ / canvasHeight_, 0.1f, 100.0f);
		camera.transform().setLocalPosition({10, 10, 10});
		camera.transform().lookAt({0, 0, 0}, {0, 1, 0});
	}

	void loadFaceData(const char *path, GLenum target)
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
		glGenTextures(1, &texture.handle);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, texture.handle);

		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAX_LEVEL, 0);

		loadFaceData(dataPath("textures/skyboxes/deep-space/+x.png").c_str(), GL_TEXTURE_CUBE_MAP_POSITIVE_X);
		loadFaceData(dataPath("textures/skyboxes/deep-space/-x.png").c_str(), GL_TEXTURE_CUBE_MAP_NEGATIVE_X);
		loadFaceData(dataPath("textures/skyboxes/deep-space/+y.png").c_str(), GL_TEXTURE_CUBE_MAP_POSITIVE_Y);
		loadFaceData(dataPath("textures/skyboxes/deep-space/-y.png").c_str(), GL_TEXTURE_CUBE_MAP_NEGATIVE_Y);
		loadFaceData(dataPath("textures/skyboxes/deep-space/+z.png").c_str(), GL_TEXTURE_CUBE_MAP_POSITIVE_Z);
		loadFaceData(dataPath("textures/skyboxes/deep-space/-z.png").c_str(), GL_TEXTURE_CUBE_MAP_NEGATIVE_Z);

		glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	}

	void initShaders()
	{
		static SkyboxDemo::Shaders shaders;

		skyboxShader = std::make_shared<ShaderProgram>(shaders.vertex.skybox, shaders.fragment.skybox);
		meshShader = std::make_shared<ShaderProgram>(shaders.vertex.simple, shaders.fragment.simple);
	}

	void cleanup() override final
	{
		glDeleteTextures(1, &texture.handle);
	}

	void render() override final
	{
		applySpectator(camera.transform(), device_);

		glViewport(0, 0, canvasWidth_, canvasHeight_);
		glClear(GL_DEPTH_BUFFER_BIT); // no need to clear color since we're rendering fullscreen quad anyway

		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);

		// Skybox
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, texture.handle);

		// Don't write to the depth buffer (the quad is always closest to the camera and prevents other objects from showing)
		glDepthMask(GL_FALSE);

		skyboxShader->use();
		skyboxShader->setMatrixUniform("projMatrix", glm::value_ptr(camera.projMatrix()));
		skyboxShader->setMatrixUniform("viewMatrix", glm::value_ptr(camera.viewMatrix()));
		skyboxShader->setTextureUniform("skyboxTex", 0);

		quadMesh->draw();

		// Mesh

		glDepthMask(GL_TRUE);

		meshShader->use();
		meshShader->setMatrixUniform("viewProjMatrix", glm::value_ptr(camera.viewProjMatrix()));
		meshShader->setMatrixUniform("worldMatrix", glm::value_ptr(meshTransform.worldMatrix()));

		boxMesh->draw();
	}
};

int main()
{
	App().run();
	return 0;
}
