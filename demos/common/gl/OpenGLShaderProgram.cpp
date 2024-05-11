/**
 * Copyright (c) Aleksey Fedotov
 * MIT licence
 */

#include "OpenGLShaderProgram.h"
#include "../Common.h"
#include <vector>

static auto compileShader(GLuint type, const void *src, uint32_t length) -> GLint
{
    static std::unordered_map<GLuint, std::string> typeNames =
        {
            {GL_VERTEX_SHADER, "vertex"},
            {GL_FRAGMENT_SHADER, "fragment"}};

    const auto shader = glCreateShader(type);

    GLint len = length;
    glShaderSource(shader, 1, reinterpret_cast<const GLchar *const *>(&src), &len);
    glCompileShader(shader);

    GLint status;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
    if (status == GL_FALSE)
    {
        GLint logLength;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);
        std::vector<GLchar> log(logLength);
        glGetShaderInfoLog(shader, logLength, nullptr, log.data());
        glDeleteShader(shader);
        panic("Unable to compile ", typeNames[type], " shader:\n", log.data());
    }

    return shader;
}

static auto linkProgram(GLuint vs, GLuint fs) -> GLint
{
    const auto program = glCreateProgram();
    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);

    GLint status;
    glGetProgramiv(program, GL_LINK_STATUS, &status);
    if (status == GL_FALSE)
    {
        GLint logLength;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLength);
        std::vector<GLchar> log(logLength);
        glGetProgramInfoLog(program, logLength, nullptr, log.data());
        glDeleteProgram(program);
        panic("Unable to link program:\n", log.data());
    }

    return program;
}

gl::ShaderProgram::ShaderProgram(const std::string &vertex, const std::string &fragment)
{
    const auto vs = compileShader(GL_VERTEX_SHADER, vertex.c_str(), vertex.size());
    const auto fs = compileShader(GL_FRAGMENT_SHADER, fragment.c_str(), fragment.size());
    handle_ = linkProgram(vs, fs);

    glDetachShader(handle_, vs);
    glDeleteShader(vs);
    glDetachShader(handle_, fs);
    glDeleteShader(fs);

    introspectAttributes();
    introspectUniforms();
}

gl::ShaderProgram::~ShaderProgram()
{
    glDeleteProgram(handle_);
}

void gl::ShaderProgram::use() const
{
    glUseProgram(handle_);
}

void gl::ShaderProgram::setMatrixUniform(const std::string &name, const float *data)
{
    const auto info = uniformInfo(name);
    glUniformMatrix4fv(info.location, 1, GL_FALSE, data);
}

void gl::ShaderProgram::setTextureUniform(const std::string &name, uint32_t slot)
{
    const auto info = uniformInfo(name);
    glUniform1i(info.location, slot);
}

auto gl::ShaderProgram::uniformInfo(const std::string &name) -> UniformInfo
{
    if (uniforms_.count(name))
        return uniforms_.at(name);
    panic("Uniform ", name, " not found");
    return {};
}

auto gl::ShaderProgram::attributeInfo(const std::string &name) -> AttributeInfo
{
    if (attributes_.count(name))
        return attributes_.at(name);
    panic("Attribute ", name, " not found");
    return {};
}

void gl::ShaderProgram::introspectUniforms()
{
    GLint activeUniforms;
    glGetProgramiv(handle_, GL_ACTIVE_UNIFORMS, &activeUniforms);
    if (activeUniforms <= 0)
        return;

    GLint nameMaxLength;
    glGetProgramiv(handle_, GL_ACTIVE_UNIFORM_MAX_LENGTH, &nameMaxLength);
    if (nameMaxLength <= 0)
        return;

    std::vector<GLchar> nameArr(nameMaxLength + 1);
    uint32_t samplerIndex = 0;
    for (GLint i = 0; i < activeUniforms; ++i)
    {
        GLint size;
        GLenum type;
        glGetActiveUniform(handle_, i, nameMaxLength, nullptr, &size, &type, nameArr.data());

        nameArr[nameMaxLength] = '\0';
        std::string name = nameArr.data();

        // Strip away possible square brackets for array uniforms,
        // they are sometimes present on some platforms
        const auto bracketIndex = name.find('[');
        if (bracketIndex != std::string::npos)
            name.erase(bracketIndex);

        uniforms_[name].location = glGetUniformLocation(handle_, nameArr.data());
        uniforms_.at(name).samplerIndex = 0;

        uint32_t idx = 0;
        if (type == GL_SAMPLER_2D || type == GL_SAMPLER_CUBE) // TODO other types of samplers
        {
            idx = samplerIndex;
            samplerIndex += size;
            uniforms_.at(name).samplerIndex = idx;
        }
    }
}

void gl::ShaderProgram::introspectAttributes()
{
    GLint activeAttributes;
    glGetProgramiv(handle_, GL_ACTIVE_ATTRIBUTES, &activeAttributes);

    GLint nameMaxLength;
    glGetProgramiv(handle_, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &nameMaxLength);
    if (nameMaxLength <= 0)
        return;

    std::vector<GLchar> nameArr(nameMaxLength + 1);
    for (GLint i = 0; i < activeAttributes; ++i)
    {
        GLint size;
        GLenum type;
        glGetActiveAttrib(handle_, i, nameMaxLength, nullptr, &size, &type, nameArr.data());

        std::string name = nameArr.data();
        attributes_[name].location = glGetAttribLocation(handle_, name.c_str());
    }
}
