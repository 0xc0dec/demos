/*
    Copyright (c) Aleksey Fedotov
    MIT license
*/

#include "common/Camera.h"
#include "common/Spectator.h"
#include "common/Mesh.h"
#include "common/gl/OpenGLAppBase.h"
#include "common/gl/ShaderProgram.h"
#include "Shaders.h"
#include <memory>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class App final : public gl::AppBase
{
public:
	App(): gl::AppBase(1366, 768, false)
	{
	}

private:
	std::shared_ptr<Mesh> mesh_;
	std::shared_ptr<gl::ShaderProgram> shader_;

	Camera camera_;
	Transform root_;
	Transform t1_, t2_, t3_;

	void init() override
	{
		initShaders();

		mesh_ = Mesh::box();

		t2_.setLocalPosition({3, 3, 3});
		t2_.lookAt({0, 0, 0}, {0, 1, 0});
		t2_.setParent(&root_);

		t3_.setLocalPosition({0, 3, 0});
		t3_.setLocalScale({0.5f, 0.5f, 0.5f});
		t3_.setParent(&t2_);

		camera_.setPerspective(45, 1.0f * device()->canvasWidth() / device()->canvasHeight(), 0.1f, 100.0f);
		camera_.transform().setLocalPosition({10, 10, 10});
		camera_.transform().lookAt({0, 0, 0}, {0, 1, 0});
	}

	void render() override
	{
		applySpectator(camera_.transform(), *device());
		
		const auto dt = device()->timeDelta();
		const auto deltaAngle = glm::radians(100 * dt);
		root_.rotate({0, 1, 0}, deltaAngle, TransformSpace::World);
		t1_.rotate({0, 0, 1}, deltaAngle, TransformSpace::Self);
		t2_.rotate({0, 0, 1}, deltaAngle, TransformSpace::Self);
		t3_.rotate({0, 1, 0}, deltaAngle, TransformSpace::Parent);

		glViewport(0, 0, device()->canvasWidth(), device()->canvasHeight());
		glClearColor(0, 0.5f, 0.6f, 1);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glDisable(GL_CULL_FACE);
		glDepthMask(GL_TRUE);
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);

		shader_->use();
		shader_->setMatrixUniform("viewProjMatrix", glm::value_ptr(camera_.viewProjMatrix()));
		
		shader_->setMatrixUniform("worldMatrix", glm::value_ptr(t1_.worldMatrix()));
		mesh_->draw();

		shader_->setMatrixUniform("worldMatrix", glm::value_ptr(t2_.worldMatrix()));
		mesh_->draw();

		shader_->setMatrixUniform("worldMatrix", glm::value_ptr(t3_.worldMatrix()));
		mesh_->draw();
	}

	void cleanup() override
	{
	}

	void initShaders()
	{
		static Shaders shaders;
		shader_ = std::make_shared<gl::ShaderProgram>(shaders.vertex.simple, shaders.fragment.simple);
	}
};

int main()
{
	App().run();
	return 0;
}
