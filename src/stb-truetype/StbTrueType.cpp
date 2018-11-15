/*
    Copyright (c) Aleksey Fedotov
    MIT license
*/

#include "common/DemoBase.h"
#include "common/Device.h"
#include "common/Common.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#define STB_TRUETYPE_IMPLEMENTATION
#include <stb_truetype.h>
#include <memory>

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

            uniform sampler2D mainTex;

            in vec2 uv0;
            out vec4 fragColor;

            void main()
            {
                vec4 c = texture(mainTex, uv0);
                fragColor = vec4(c.r, c.r, c.r, c.r);
            }
        )";
	} fragment;
} shaders;

struct GlyphInfo
{
	glm::vec3 positions[4];
	glm::vec2 uvs[4];
	float offsetX = 0;
	float offsetY = 0;
};

class Demo final : public DemoBase
{
public:
	Demo(int canvasWidth, int canvasHeight):
		DemoBase(canvasWidth, canvasHeight, false)
	{
	}

private:
	struct
	{
		GLuint handle = 0;

		struct
		{
			GLuint viewProjMatrix = 0;
			GLuint worldMatrix = 0;
			GLuint texture = 0;
		} uniforms;
	} program;

	glm::mat4 viewProjMatrix;

	struct
	{
		GLuint vao = 0;
		GLuint vertexBuffer = 0;
		GLuint uvBuffer = 0;
		GLuint indexBuffer = 0;
		uint16_t indexElementCount = 0;
		float angle = 0;
	} rotatingLabel;

	struct
	{
		GLuint vao = 0;
		GLuint vertexBuffer = 0;
		GLuint uvBuffer = 0;
		float time = 0;
	} atlasQuad;

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
	} font;

	void init() override final
	{
		initFont();
		initRotatingLabel();
		initAtlasQuad();
		initProgram();
		initUniforms();
	}

	void shutdown() override final
	{
		glDeleteVertexArrays(1, &rotatingLabel.vao);
		glDeleteBuffers(1, &rotatingLabel.vertexBuffer);
		glDeleteBuffers(1, &rotatingLabel.uvBuffer);
		glDeleteBuffers(1, &rotatingLabel.indexBuffer);
		glDeleteVertexArrays(1, &atlasQuad.vao);
		glDeleteBuffers(1, &atlasQuad.vertexBuffer);
		glDeleteBuffers(1, &atlasQuad.uvBuffer);
		glDeleteTextures(1, &font.texture);
		glDeleteProgram(program.handle);
	}

	void render() override final
	{
		glViewport(0, 0, canvasWidth, canvasHeight);
		glClearColor(0, 0.5f, 0.6f, 1);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glDisable(GL_CULL_FACE);
		glDepthMask(GL_TRUE);
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glUseProgram(program.handle);

		glUniformMatrix4fv(program.uniforms.viewProjMatrix, 1, GL_FALSE, glm::value_ptr(viewProjMatrix));

		glBindTexture(GL_TEXTURE_2D, font.texture);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST_MIPMAP_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 8);
		glActiveTexture(GL_TEXTURE0);
		glUniform1i(program.uniforms.texture, 0);

		const auto dt = device.getTimeDelta();
		renderRotatingLabel(dt);
		renderAtlasQuad(dt);
	}

	void initProgram()
	{
		program.handle = createProgram(shaders.vertex.font, shaders.fragment.font);
		glUseProgram(program.handle);
	}

	void initFont()
	{
		auto fontData = readFile("C:/windows/fonts/arial.ttf");
		auto atlasData = std::make_unique<uint8_t[]>(font.atlasWidth * font.atlasHeight);

		font.charInfo = std::make_unique<stbtt_packedchar[]>(font.charCount);

		stbtt_pack_context context;
		if (!stbtt_PackBegin(&context, atlasData.get(), font.atlasWidth, font.atlasHeight, 0, 1, nullptr))
			DIE("Failed to initialize font");

		stbtt_PackSetOversampling(&context, font.oversampleX, font.oversampleY);
		if (!stbtt_PackFontRange(&context, fontData.data(), 0, font.size, font.firstChar, font.charCount, font.charInfo.get()))
			DIE("Failed to pack font");

		stbtt_PackEnd(&context);

		glGenTextures(1, &font.texture);
		glBindTexture(GL_TEXTURE_2D, font.texture);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, font.atlasWidth, font.atlasHeight, 0, GL_RED, GL_UNSIGNED_BYTE, atlasData.get());
		glHint(GL_GENERATE_MIPMAP_HINT, GL_NICEST);
		glGenerateMipmap(GL_TEXTURE_2D);
	}

	void initUniforms()
	{
		const glm::mat4 viewMatrix{};
		const auto projMatrix = glm::perspective(glm::radians(60.0f), 1.0f * canvasWidth / canvasHeight, 0.05f, 100.0f);
		viewProjMatrix = projMatrix * viewMatrix;

		program.uniforms.viewProjMatrix = glGetUniformLocation(program.handle, "viewProjMatrix");
		program.uniforms.worldMatrix = glGetUniformLocation(program.handle, "worldMatrix");
		program.uniforms.texture = glGetUniformLocation(program.handle, "mainTex");
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
			const auto glyphInfo = getGlyphInfo(c, offsetX, offsetY);
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

		glGenVertexArrays(1, &rotatingLabel.vao);
		glBindVertexArray(rotatingLabel.vao);

		glGenBuffers(1, &rotatingLabel.vertexBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, rotatingLabel.vertexBuffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * vertices.size(), vertices.data(), GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
		glEnableVertexAttribArray(0);

		glGenBuffers(1, &rotatingLabel.uvBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, rotatingLabel.uvBuffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * uvs.size(), uvs.data(), GL_STATIC_DRAW);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
		glEnableVertexAttribArray(1);

		rotatingLabel.indexElementCount = indexes.size();
		glGenBuffers(1, &rotatingLabel.indexBuffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, rotatingLabel.indexBuffer);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint16_t) * rotatingLabel.indexElementCount, indexes.data(), GL_STATIC_DRAW);
	}

	void initAtlasQuad()
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

		glGenVertexArrays(1, &atlasQuad.vao);
		glBindVertexArray(atlasQuad.vao);

		glGenBuffers(1, &atlasQuad.vertexBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, atlasQuad.vertexBuffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * 18, vertices, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
		glEnableVertexAttribArray(0);

		glGenBuffers(1, &atlasQuad.uvBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, atlasQuad.uvBuffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 2 * 12, uvs, GL_STATIC_DRAW);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
		glEnableVertexAttribArray(1);
	}

	void renderRotatingLabel(float dt)
	{
		rotatingLabel.angle += dt;

		const auto worldMatrix = glm::scale(
			glm::rotate(
				glm::translate(glm::mat4{}, {0, 5, -30}),
				rotatingLabel.angle,
				{0, 1, 0}),
			{0.05f, 0.05f, 1});
		glUniformMatrix4fv(program.uniforms.worldMatrix, 1, GL_FALSE, glm::value_ptr(worldMatrix));

		glBindVertexArray(rotatingLabel.vao);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, rotatingLabel.indexBuffer);
		glDrawElements(GL_TRIANGLES, rotatingLabel.indexElementCount, GL_UNSIGNED_SHORT, nullptr);
	}

	void renderAtlasQuad(float dt)
	{
		atlasQuad.time += dt;
		const auto distance = -10 - 5 * sinf(atlasQuad.time);

		const auto worldMatrix = glm::scale(glm::translate(glm::mat4{}, {0, -6, distance}), {6, 6, 1});
		glUniformMatrix4fv(program.uniforms.worldMatrix, 1, GL_FALSE, glm::value_ptr(worldMatrix));

		glBindVertexArray(atlasQuad.vao);
		glDrawArrays(GL_TRIANGLES, 0, 6); // 6 vertices
	}

	auto getGlyphInfo(uint32_t character, float offsetX, float offsetY) -> GlyphInfo
	{
		stbtt_aligned_quad quad;

		stbtt_GetPackedQuad(font.charInfo.get(), font.atlasWidth, font.atlasHeight,
			character - font.firstChar, &offsetX, &offsetY, &quad, 1);
		auto xmin = quad.x0;
		auto xmax = quad.x1;
		auto ymin = -quad.y1;
		auto ymax = -quad.y0;

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
	Demo demo{800, 600};
	demo.run();
	return 0;
}
