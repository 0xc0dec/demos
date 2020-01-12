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

class App final : public AppBase
{
public:
	App(): AppBase(1366, 768, false)
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
			GLuint viewProjMatrix = 0;
			GLuint worldMatrix = 0;
		} uniforms;
	} shader;

	Camera camera;
	Transform root;
	Transform t1, t2, t3;

	void init() override final
	{
		initShaders();
		initQuadMesh();

		t2.setLocalPosition({3, 3, 3});
		t2.lookAt({0, 0, 0}, {0, 1, 0});
		t2.setParent(&root);

		t3.setLocalPosition({0, 3, 0});
		t3.setLocalScale({0.5f, 0.5f, 0.5f});
		t3.setParent(&t2);

		camera.setPerspective(45, 1.0f * canvasWidth_ / canvasHeight_, 0.1f, 100.0f);
		camera.transform().setLocalPosition({10, 10, 10});
		camera.transform().lookAt({0, 0, 0}, {0, 1, 0});
	}

	void cleanup() override final
	{
		cleanupQuadMesh();
		glDeleteProgram(shader.handle);
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

		glUseProgram(shader.handle);
		glUniformMatrix4fv(shader.uniforms.viewProjMatrix, 1, GL_FALSE, glm::value_ptr(camera.viewProjMatrix()));
		
		glUniformMatrix4fv(shader.uniforms.worldMatrix, 1, GL_FALSE, glm::value_ptr(t1.worldMatrix()));
		drawQuadMesh();

		glUniformMatrix4fv(shader.uniforms.worldMatrix, 1, GL_FALSE, glm::value_ptr(t2.worldMatrix()));
		drawQuadMesh();

		glUniformMatrix4fv(shader.uniforms.worldMatrix, 1, GL_FALSE, glm::value_ptr(t3.worldMatrix()));
		drawQuadMesh();
	}

	void initShaders()
	{
		static TransformDemo::Shaders shaders;
		shader.handle = createProgram(shaders.vertex.font, shaders.fragment.font);
		shader.uniforms.viewProjMatrix = glGetUniformLocation(shader.handle, "viewProjMatrix");
		shader.uniforms.worldMatrix = glGetUniformLocation(shader.handle, "worldMatrix");
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

	void cleanupQuadMesh()
	{
		glDeleteVertexArrays(1, &quadMesh.vao);
		glDeleteBuffers(1, &quadMesh.vertexBuffer);
		glDeleteBuffers(1, &quadMesh.uvBuffer);
	}

	void drawQuadMesh()
	{
		glBindVertexArray(quadMesh.vao);
		glDrawArrays(GL_TRIANGLES, 0, 6);
	}
};

int main()
{
	App().run();
	return 0;
}
