/*
    Copyright (c) Aleksey Fedotov
    MIT license
*/

#include "common/AppBase.h"
#include "common/Device.h"
#include "common/Common.h"
#include "common/Camera.h"
#include "common/Spectator.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

static struct
{
	struct
	{
		const char* font = R"(
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
		const char* font = R"(
            #version 330 core

            in vec2 uv0;
            out vec4 fragColor;

            void main()
            {
                fragColor = vec4(uv0.x, uv0.y, 0, 1);
            }
        )";
	} fragment;
} shaders;

class Demo final : public AppBase
{
public:
	Demo(int canvasWidth, int canvasHeight):
		AppBase(canvasWidth, canvasHeight, false)
	{
	}

private:
	struct
	{
		GLuint vao = 0;
		GLuint vertexBuffer = 0;
		GLuint uvBuffer = 0;
	} quad;

	struct
	{
		GLuint handle = 0;

		struct
		{
			GLuint viewProjMatrix = 0;
			GLuint worldMatrix = 0;
		} uniforms;
	} program;

	Camera camera;
	Transform root;
	Transform t1, t2, t3;

	void init() override final
	{
		initProgram();
		initQuad();

		t2.setLocalPosition({3, 3, 3});
		t2.lookAt({0, 0, 0}, {0, 1, 0});
		t2.setParent(&root);

		t3.setLocalPosition({0, 3, 0});
		t3.setLocalScale({0.5f, 0.5f, 0.5f});
		t3.setParent(&t2);

		camera.transform().setLocalPosition({10, 10, 10});
		camera.transform().lookAt({0, 0, 0}, {0, 1, 0});
	}

	void shutdown() override final
	{
		destroyQuad();
		destroyProgram();
	}

	void render() override final
	{
		applySpectator(camera.transform(), device_);
		
		const auto dt = device_.timeDelta();
		const auto deltaAngle = glm::radians(100 * dt);
		root.rotate({0, 1, 0}, deltaAngle, TransformSpace::World);
		t1.rotate({0, 0, 1}, deltaAngle, TransformSpace::Self);
		t2.rotate({0, 0, 1}, deltaAngle, TransformSpace::Self);
		t3.rotate({0, 1, 0}, deltaAngle, TransformSpace::Parent);

		glViewport(0, 0, canvasWidth_, canvasHeight_);
		glClearColor(0, 0.5f, 0.6f, 1);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glDisable(GL_CULL_FACE);
		glDepthMask(GL_TRUE);
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glUseProgram(program.handle);
		glUniformMatrix4fv(program.uniforms.viewProjMatrix, 1, GL_FALSE, glm::value_ptr(camera.viewProjMatrix()));
		
		glUniformMatrix4fv(program.uniforms.worldMatrix, 1, GL_FALSE, glm::value_ptr(t1.worldMatrix()));
		drawQuad();

		glUniformMatrix4fv(program.uniforms.worldMatrix, 1, GL_FALSE, glm::value_ptr(t2.worldMatrix()));
		drawQuad();

		glUniformMatrix4fv(program.uniforms.worldMatrix, 1, GL_FALSE, glm::value_ptr(t3.worldMatrix()));
		drawQuad();
	}

	void initProgram()
	{
		program.handle = createProgram(shaders.vertex.font, shaders.fragment.font);
		program.uniforms.viewProjMatrix = glGetUniformLocation(program.handle, "viewProjMatrix");
		program.uniforms.worldMatrix = glGetUniformLocation(program.handle, "worldMatrix");
	}

	void destroyProgram()
	{
		glDeleteProgram(program.handle);
	}

	void initQuad()
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

		glGenVertexArrays(1, &quad.vao);
		glBindVertexArray(quad.vao);

		glGenBuffers(1, &quad.vertexBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, quad.vertexBuffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * 18, vertices, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
		glEnableVertexAttribArray(0);

		glGenBuffers(1, &quad.uvBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, quad.uvBuffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 2 * 12, uvs, GL_STATIC_DRAW);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
		glEnableVertexAttribArray(1);
	}

	void destroyQuad()
	{
		glDeleteVertexArrays(1, &quad.vao);
		glDeleteBuffers(1, &quad.vertexBuffer);
		glDeleteBuffers(1, &quad.uvBuffer);
	}

	void drawQuad()
	{
		glBindVertexArray(quad.vao);
		glDrawArrays(GL_TRIANGLES, 0, 6);
	}
};

int main()
{
	Demo demo{800, 600};
	demo.run();
	return 0;
}
