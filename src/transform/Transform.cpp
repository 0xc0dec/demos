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

class App final : public AppBase
{
public:
	App(): AppBase(1366, 768, false)
	{
	}

private:
	std::shared_ptr<Mesh> mesh;
	std::shared_ptr<ShaderProgram> shader;

	Camera camera;
	Transform root;
	Transform t1, t2, t3;

	void init() override final
	{
		initShaders();

		mesh = Mesh::box();

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

		shader->use();
		shader->setMatrixUniform("viewProjMatrix", glm::value_ptr(camera.viewProjMatrix()));
		
		shader->setMatrixUniform("worldMatrix", glm::value_ptr(t1.worldMatrix()));
		mesh->draw();

		shader->setMatrixUniform("worldMatrix", glm::value_ptr(t2.worldMatrix()));
		mesh->draw();

		shader->setMatrixUniform("worldMatrix", glm::value_ptr(t3.worldMatrix()));
		mesh->draw();
	}

	void initShaders()
	{
		static TransformDemo::Shaders shaders;
		shader = std::make_shared<ShaderProgram>(shaders.vertex.simple, shaders.fragment.simple);
	}
};

int main()
{
	App().run();
	return 0;
}
