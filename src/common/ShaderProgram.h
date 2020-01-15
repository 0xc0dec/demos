#pragma once

#include <unordered_map>
#include <string>
#include <GL/glew.h>

class ShaderProgram
{
public:
    ShaderProgram(const std::string &vertex, const std::string &fragment);
    ~ShaderProgram();

    void use();

    void setMatrixUniform(const std::string &name, const float *data);
    void setTextureUniform(const std::string &name, uint32_t slot);

private:
    struct UniformInfo
    {
        uint32_t location;
        uint32_t samplerIndex;
    };

    struct AttributeInfo
    {
        uint32_t location;
    };

    GLuint handle_ = 0;
    std::unordered_map<std::string, UniformInfo> uniforms_;
    std::unordered_map<std::string, AttributeInfo> attributes_;

    void introspectUniforms();
    void introspectAttributes();

    auto uniformInfo(const std::string &name) -> UniformInfo;
    auto attributeInfo(const std::string &name) -> AttributeInfo;
};