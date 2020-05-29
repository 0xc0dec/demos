#include "Mesh.h"

Mesh::Mesh(const std::vector<float> &positions, const std::vector<float> &uvs)
{
    glGenVertexArrays(1, &vao_);
    glBindVertexArray(vao_);

    buffers_.resize(2);

    // Positions
    glGenBuffers(1, &buffers_[0]);
    glBindBuffer(GL_ARRAY_BUFFER, buffers_[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * positions.size(), positions.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(0);

    // Texture coordinates
    glGenBuffers(1, &buffers_[1]);
    glBindBuffer(GL_ARRAY_BUFFER, buffers_[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * uvs.size(), uvs.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(1);

    verticesCount_ = positions.size() / 3;
}

Mesh::~Mesh()
{
    glDeleteVertexArrays(1, &vao_);
    for (const auto &buffer: buffers_)
        glDeleteBuffers(1, &buffer);
}

void Mesh::draw() const
{
    glBindVertexArray(vao_);
    glDrawArrays(GL_TRIANGLES, 0, verticesCount_);
}

auto Mesh::quad() -> std::shared_ptr<Mesh>
{
    static std::vector<float> positions = {
        -1, -1, 0,
        -1,  1, 0,
         1,  1, 0,
        -1, -1, 0,
         1,  1, 0,
         1, -1, 0,
    };

    static std::vector<float> uvs = {
        0, 1,
        0, 0,
        1, 0,
        0, 1,
        1, 0,
        1, 1,
    };

    return std::make_shared<Mesh>(positions, uvs);
}

auto Mesh::box() -> std::shared_ptr<Mesh>
{
    static std::vector<float> positions = {
        // -x
        -1, -1, -1,    -1,  1, -1,    -1,  1,  1,
        -1, -1, -1,    -1,  1,  1,    -1, -1,  1,
                                               
        // +x                                  
         1,  1, -1,     1, -1, -1,     1,  -1,  1,
         1,  1, -1,     1, -1,  1,     1,   1,  1,
                                               
        // -y                                  
        -1, -1, -1,     1, -1, -1,     1, -1,  1,
        -1, -1, -1,     1, -1,  1,    -1, -1,  1,
                                               
        // +y                                  
        -1,  1, -1,     1,  1, -1,     1,  1,  1,
        -1,  1, -1,     1,  1,  1,    -1,  1,  1,

        // -z
        -1,  1, -1,    -1, -1, -1,     1, -1, -1,
        -1,  1, -1,     1, -1, -1,     1,  1, -1,

        // +z
         1,  1,  1,     1, -1,  1,    -1, -1,  1,
         1,  1,  1,    -1, -1,  1,    -1,  1,  1
    };

    static std::vector<float> uvs = {
        // -x
        1, 1,    0, 1,    0, 0,
        1, 1,    0, 0,    1, 0,

        // +x
        1, 1,    0, 1,    0, 0,
        1, 1,    0, 0,    1, 0,

        // -y
        1, 1,    0, 1,    0, 0,
        1, 1,    0, 0,    1, 0,

        // +y
        1, 1,    0, 1,    0, 0,
        1, 1,    0, 0,    1, 0,

        // -z
        1, 1,    0, 1,    0, 0,
        1, 1,    0, 0,    1, 0,

        // +z
        1, 1,    0, 1,    0, 0,
        1, 1,    0, 0,    1, 0,
    };

    return std::make_shared<Mesh>(positions, uvs);
}
