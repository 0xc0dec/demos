#pragma once

#include <memory>
#include <vector>
#include <GL/glew.h>

class Mesh
{
public:
    Mesh(const std::vector<float> &positions, const std::vector<float> &uvs);
    ~Mesh();

    void draw();

    static auto quad() -> std::shared_ptr<Mesh>;
    static auto box() -> std::shared_ptr<Mesh>;

private:

    int32_t verticesCount_ = 0;
    GLuint vao_ = 0;
    GLuint vertexBuffer_ = 0;
    GLuint uvBuffer_ = 0;
};
