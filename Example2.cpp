#include "common/ExampleBase.h"
#include "common/Matrix.h"
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
	            fragColor = vec4(c.r, c.r, c.r, 1 - c.r);
	        }
	    )";
    } fragment;
} shaders;


class Example final: public ExampleBase
{
public:
    Example(int canvasWidth, int canvasHeight, bool fullScreen):
        ExampleBase(canvasWidth, canvasHeight, fullScreen)
    {
    }

private:
    void initProgram();
    void initFont();
    void initUniforms();
    void initTextQuad();
    void renderTextQuad();

    void init() override final;
    void shutdown() override final;
    void render(float dt) override final;

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

    Matrix viewProjMatrix;

    struct
    {
        GLuint vao = 0;
        GLuint vertexBuffer = 0;
        GLuint uvBuffer = 0;
    } textQuad;

    struct
    {
        const uint32_t size = 120;
        const uint32_t textureWidth = 1024;
        const uint32_t textureHeight = 1024;
        GLuint texture = 0;
    } font;
};


void Example::initProgram()
{
    program.handle = createProgram(shaders.vertex.font, shaders.fragment.font);
    glUseProgram(program.handle);
}


void Example::initFont()
{
    auto fontData = readFile("C:/windows/fonts/arial.ttf");

    stbtt_fontinfo fontInfo;
    stbtt_InitFont(&fontInfo, fontData.data(), 0);

    int ascent, descent, lineGap, x = 0;
    stbtt_GetFontVMetrics(&fontInfo, &ascent, &descent, &lineGap);

    auto scale = stbtt_ScaleForPixelHeight(&fontInfo, font.size);
    ascent *= scale;

    auto pixels = std::make_unique<uint8_t[]>(font.textureWidth * font.textureHeight);

    const std::string text = "Baked string";

    for (size_t i = 0; i < text.size(); ++i)
    {
        int x1, y1, x2, y2;
        stbtt_GetCodepointBitmapBox(&fontInfo, text[i], scale, scale, &x1, &y1, &x2, &y2);

        auto y = ascent + y1;
    
        int byteOffset = x + y  * font.textureWidth;
        stbtt_MakeCodepointBitmap(&fontInfo, pixels.get() + byteOffset, x2 - x1, y2 - y1, font.textureWidth, scale, scale, text[i]);
    
        int ax;
        stbtt_GetCodepointHMetrics(&fontInfo, text[i], &ax, nullptr);
        x += ax * scale;

        if (i != text.size() - 1)
        {
            auto kern = stbtt_GetCodepointKernAdvance(&fontInfo, text[i], text[i + 1]);
            x += kern * scale;
        }
    }

    glGenTextures(1, &font.texture);
    glBindTexture(GL_TEXTURE_2D, font.texture);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, font.textureWidth, font.textureHeight, 0, GL_RED, GL_UNSIGNED_BYTE, pixels.get());
    glHint(GL_GENERATE_MIPMAP_HINT, GL_NICEST);
    glGenerateMipmap(GL_TEXTURE_2D);
}


void Example::initUniforms()
{
    viewProjMatrix = Matrix::createPerspective(60, 1.0f * canvasWidth / canvasHeight, 0.05f, 100.0f);

    program.uniforms.viewProjMatrix = glGetUniformLocation(program.handle, "viewProjMatrix");
    program.uniforms.worldMatrix = glGetUniformLocation(program.handle, "worldMatrix");
    program.uniforms.texture = glGetUniformLocation(program.handle, "mainTex");
}


void Example::initTextQuad()
{
    const float vertices[] =
    {
        -1, -1, 0,
        -1,  1, 0,
         1,  1, 0,
        -1, -1, 0,
         1,  1, 0,
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

    glGenVertexArrays(1, &textQuad.vao);
    glBindVertexArray(textQuad.vao);

    glGenBuffers(1, &textQuad.vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, textQuad.vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * 18, vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(0);

    glGenBuffers(1, &textQuad.uvBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, textQuad.uvBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 2 * 12, uvs, GL_STATIC_DRAW);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(1);
}


void Example::renderTextQuad()
{
    auto worldMatrix = Matrix::createTranslation(Vector3(0, 0, -15));
    worldMatrix.scaleByVector(Vector3(6, 6, 1));
    glUniformMatrix4fv(program.uniforms.worldMatrix, 1, GL_FALSE, worldMatrix.m);

    glBindVertexArray(textQuad.vao);
    glDrawArrays(GL_TRIANGLES, 0, 6); // 6 vertices
}


void Example::init()
{
    initFont();
    initTextQuad();
    initProgram();
    initUniforms();
}


void Example::shutdown()
{
    glDeleteVertexArrays(1, &textQuad.vao);
    glDeleteBuffers(1, &textQuad.vertexBuffer);
    glDeleteBuffers(1, &textQuad.uvBuffer);
    glDeleteTextures(1, &font.texture);
    glDeleteProgram(program.handle);
}


void Example::render(float dt)
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

    glUniformMatrix4fv(program.uniforms.viewProjMatrix, 1, GL_FALSE, viewProjMatrix.m);

    glBindTexture(GL_TEXTURE_2D, font.texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST_MIPMAP_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 8);
    glActiveTexture(GL_TEXTURE0);
    glUniform1i(program.uniforms.texture, 0);

    renderTextQuad();
}


int main()
{
    Example example{ 800, 600, false };
    example.run();
    return 0;
}
