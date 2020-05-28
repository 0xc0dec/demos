/*
    Copyright (c) Aleksey Fedotov
    MIT license
*/

#include "common/AppBase.h"
#include "common/Device.h"
#include "common/Common.h"
#include "common/Mesh.h"
#include "common/ShaderProgram.h"
#include "Shaders.h"
#include <memory>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#define STB_TRUETYPE_IMPLEMENTATION
#include <stb_truetype.h>
#include <memory>

struct GlyphInfo
{
	glm::vec3 positions[4];
	glm::vec2 uvs[4];
	float offsetX = 0;
	float offsetY = 0;
};

class App final : public AppBase
{
public:
	App(): AppBase(1366, 768, false)
	{
	}

private:
	std::shared_ptr<ShaderProgram> shader_;

	glm::mat4 viewProjMatrix_;

	struct
	{
		GLuint vao = 0;
		GLuint vertexBuffer = 0;
		GLuint uvBuffer = 0;
		GLuint indexBuffer = 0;
		uint16_t indexElementCount = 0;
		float angle = 0;
	} rotatingLabel_;

	struct
	{
		std::shared_ptr<Mesh> mesh;
		float time = 0;
	} atlasQuad_;

	struct
	{
		const uint32_t size = 40;
		const uint32_t atlasWidth = 1024;
		const uint32_t atlasHeight = 1024;
		const uint32_t oversampleX = 2;
		const uint32_t oversampleY = 2;
		const uint32_t firstChar = ' ';
		const uint32_t charCount = '~' - ' ';
		std::unique_ptr<stbtt_packedchar[]> charInfo;
		GLuint texture = 0;
	} font_;

	void init() override
	{
		initFont();
		initRotatingLabel();
		initShaders();

		atlasQuad_.mesh = Mesh::quad();

		const glm::mat4 viewMatrix{};
		const auto projMatrix = glm::perspective(glm::radians(60.0f), 1.0f * canvasWidth() / canvasHeight(), 0.05f, 100.0f);
		viewProjMatrix_ = projMatrix * viewMatrix;
	}

	void cleanup() override
	{
		cleanupRotatingLabel();
		glDeleteTextures(1, &font_.texture);
	}

	void cleanupRotatingLabel() const
	{
		glDeleteVertexArrays(1, &rotatingLabel_.vao);
		glDeleteBuffers(1, &rotatingLabel_.vertexBuffer);
		glDeleteBuffers(1, &rotatingLabel_.uvBuffer);
		glDeleteBuffers(1, &rotatingLabel_.indexBuffer);
	}

	void render() override
	{
		glViewport(0, 0, canvasWidth(), canvasHeight());
		glClearColor(0, 0.5f, 0.6f, 1);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glDisable(GL_CULL_FACE);
		
		glDepthMask(GL_TRUE);
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		shader_->use();
		shader_->setMatrixUniform("viewProjMatrix", glm::value_ptr(viewProjMatrix_));

		glBindTexture(GL_TEXTURE_2D, font_.texture);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST_MIPMAP_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 8);
		glActiveTexture(GL_TEXTURE0);
		
		shader_->setTextureUniform("mainTex", 0);

		const auto dt = device().timeDelta();
		renderRotatingLabel(dt);
		renderAtlasQuad(dt);
	}

	void initShaders()
	{
		static StbTrueTypeDemo::Shaders shaders;
		shader_ = std::make_shared<ShaderProgram>(shaders.vertex.font, shaders.fragment.font);
	}

	void initFont()
	{
		auto fontData = readFile(dataPath("fonts/OpenSans-Regular.ttf").c_str());
		auto atlasData = std::make_unique<uint8_t[]>(font_.atlasWidth * font_.atlasHeight);

		font_.charInfo = std::make_unique<stbtt_packedchar[]>(font_.charCount);

		stbtt_pack_context context;
		if (!stbtt_PackBegin(&context, atlasData.get(), font_.atlasWidth, font_.atlasHeight, 0, 1, nullptr))
			PANIC("Failed to initialize font");

		stbtt_PackSetOversampling(&context, font_.oversampleX, font_.oversampleY);
		if (!stbtt_PackFontRange(&context, fontData.data(), 0, font_.size, font_.firstChar, font_.charCount, font_.charInfo.get()))
			PANIC("Failed to pack font");

		stbtt_PackEnd(&context);

		glGenTextures(1, &font_.texture);
		glBindTexture(GL_TEXTURE_2D, font_.texture);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, font_.atlasWidth, font_.atlasHeight, 0, GL_RED, GL_UNSIGNED_BYTE, atlasData.get());
		glHint(GL_GENERATE_MIPMAP_HINT, GL_NICEST);
		glGenerateMipmap(GL_TEXTURE_2D);
	}

	void initRotatingLabel()
	{
		const std::string text = "Rotating in world space";

		std::vector<glm::vec3> vertices;
		std::vector<glm::vec2> uvs;
		std::vector<uint16_t> indexes;

		uint16_t lastIndex = 0;
		float offsetX = 0, offsetY = 0;
		for (auto c : text)
		{
			const auto glyphInfo = makeGlyphInfo(c, offsetX, offsetY);
			offsetX = glyphInfo.offsetX;
			offsetY = glyphInfo.offsetY;

			vertices.emplace_back(glyphInfo.positions[0]);
			vertices.emplace_back(glyphInfo.positions[1]);
			vertices.emplace_back(glyphInfo.positions[2]);
			vertices.emplace_back(glyphInfo.positions[3]);
			uvs.emplace_back(glyphInfo.uvs[0]);
			uvs.emplace_back(glyphInfo.uvs[1]);
			uvs.emplace_back(glyphInfo.uvs[2]);
			uvs.emplace_back(glyphInfo.uvs[3]);
			indexes.push_back(lastIndex);
			indexes.push_back(lastIndex + 1);
			indexes.push_back(lastIndex + 2);
			indexes.push_back(lastIndex);
			indexes.push_back(lastIndex + 2);
			indexes.push_back(lastIndex + 3);

			lastIndex += 4;
		}

		glGenVertexArrays(1, &rotatingLabel_.vao);
		glBindVertexArray(rotatingLabel_.vao);

		glGenBuffers(1, &rotatingLabel_.vertexBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, rotatingLabel_.vertexBuffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * vertices.size(), vertices.data(), GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
		glEnableVertexAttribArray(0);

		glGenBuffers(1, &rotatingLabel_.uvBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, rotatingLabel_.uvBuffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * uvs.size(), uvs.data(), GL_STATIC_DRAW);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
		glEnableVertexAttribArray(1);

		rotatingLabel_.indexElementCount = indexes.size();
		glGenBuffers(1, &rotatingLabel_.indexBuffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, rotatingLabel_.indexBuffer);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint16_t) * rotatingLabel_.indexElementCount, indexes.data(), GL_STATIC_DRAW);
	}

	void renderRotatingLabel(float dt)
	{
		rotatingLabel_.angle += dt;

		auto worldMatrix = glm::translate(glm::mat4{}, {0, 5, -30});
		worldMatrix = glm::rotate(worldMatrix, rotatingLabel_.angle, {0, 1, 0});
		worldMatrix = glm::scale(worldMatrix, {0.05f, 0.05f, 1});
		shader_->setMatrixUniform("worldMatrix", glm::value_ptr(worldMatrix));

		glBindVertexArray(rotatingLabel_.vao);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, rotatingLabel_.indexBuffer);
		glDrawElements(GL_TRIANGLES, rotatingLabel_.indexElementCount, GL_UNSIGNED_SHORT, nullptr);
	}

	void renderAtlasQuad(float dt)
	{
		atlasQuad_.time += dt;
		const auto distance = -10 - 5 * sinf(atlasQuad_.time);

		auto worldMatrix = glm::translate(glm::mat4{}, {0, -6, distance});
		worldMatrix = glm::scale(worldMatrix, {6, 6, 1});
		shader_->setMatrixUniform("worldMatrix", glm::value_ptr(worldMatrix));

		atlasQuad_.mesh->draw();
	}

	auto makeGlyphInfo(uint32_t character, float offsetX, float offsetY) const -> GlyphInfo
	{
		stbtt_aligned_quad quad;

		stbtt_GetPackedQuad(font_.charInfo.get(), font_.atlasWidth, font_.atlasHeight,
			character - font_.firstChar, &offsetX, &offsetY, &quad, 1);
		const auto xmin = quad.x0;
		const auto xmax = quad.x1;
		const auto ymin = -quad.y1;
		const auto ymax = -quad.y0;

		GlyphInfo info{};
		info.offsetX = offsetX;
		info.offsetY = offsetY;
		info.positions[0] = {xmin, ymin, 0};
		info.positions[1] = {xmin, ymax, 0};
		info.positions[2] = {xmax, ymax, 0};
		info.positions[3] = {xmax, ymin, 0};
		info.uvs[0] = {quad.s0, quad.t1};
		info.uvs[1] = {quad.s0, quad.t0};
		info.uvs[2] = {quad.s1, quad.t0};
		info.uvs[3] = {quad.s1, quad.t1};

		return info;
	}
};

int main()
{
	App().run();
	return 0;
}
