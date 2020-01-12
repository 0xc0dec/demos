/*
    Copyright (c) Aleksey Fedotov
    MIT license
*/

#include "common/AppBase.h"
#include "common/Device.h"
#include "common/Common.h"
#include "common/Camera.h"
#include "common/Spectator.h"
#include "Shaders.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

class App: public AppBase
{
public:
	App(int canvasWidth, int canvasHeight) :
		AppBase(canvasWidth, canvasHeight, false)
	{
	}

private:
	struct
	{
		GLuint vao = 0;
		GLuint vertexBuffer = 0;
		GLuint uvBuffer = 0;
	} quadMesh;

	struct
	{
		GLuint handle = 0;

		struct
		{
			GLuint viewMatrix = 0;
			GLuint projMatrix = 0;
			GLuint texture = 0;
		} uniforms;
	} shader;

	struct
	{
		GLuint handle;
	} texture;

	Camera camera;

	void init() override final
	{
		initQuadMesh();
		initShaders();
		initTextures();
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
		shader.handle = createProgram(shaders.vertex.skybox, shaders.fragment.skybox);
		shader.uniforms.projMatrix = glGetUniformLocation(shader.handle, "projMatrix");
		shader.uniforms.viewMatrix = glGetUniformLocation(shader.handle, "viewMatrix");
		shader.uniforms.texture = glGetUniformLocation(shader.handle, "skyboxTex");
	}

	void initQuadMesh()
	{
		const float vertices[] =
		{
			-1, -1, 0,
			-1, 1, 0,
			1, 1, 0,
			-1, -1, 0,
			1, 1, 0,
			1, -1, 0,
		};

		const float uvs[] =
		{
			0, 1,
			0, 0,
			1, 0,
			0, 1,
			1, 0,
			1, 1,
		};

		glGenVertexArrays(1, &quadMesh.vao);
		glBindVertexArray(quadMesh.vao);

		glGenBuffers(1, &quadMesh.vertexBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, quadMesh.vertexBuffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * 18, vertices, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
		glEnableVertexAttribArray(0);

		glGenBuffers(1, &quadMesh.uvBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, quadMesh.uvBuffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 2 * 12, uvs, GL_STATIC_DRAW);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
		glEnableVertexAttribArray(1);
	}

	void cleanup() override final
	{
		glDeleteProgram(shader.handle);
		glDeleteTextures(1, &texture.handle);
		cleanupQuadMesh();
	}

	void cleanupQuadMesh()
	{
		glDeleteVertexArrays(1, &quadMesh.vao);
		glDeleteBuffers(1, &quadMesh.vertexBuffer);
		glDeleteBuffers(1, &quadMesh.uvBuffer);
	}

	void render() override final
	{
		applySpectator(camera.transform(), device_);

		glViewport(0, 0, canvasWidth_, canvasHeight_);
		glClear(GL_DEPTH_BUFFER_BIT); // no need to clear color since we're rendering fullscreen quad anyway

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, texture.handle);
		
		glUseProgram(shader.handle);

		glUniformMatrix4fv(shader.uniforms.projMatrix, 1, GL_FALSE, glm::value_ptr(camera.projMatrix()));
		glUniformMatrix4fv(shader.uniforms.viewMatrix, 1, GL_FALSE, glm::value_ptr(camera.viewMatrix()));
		glUniform1i(shader.uniforms.texture, 0);

		// Draw mesh
		glBindVertexArray(quadMesh.vao);
		glDrawArrays(GL_TRIANGLES, 0, 6);
	}
};

int main()
{
	App app{1366, 768};
	app.run();
	return 0;
}
