#include "Meshes.h"

Mesh::Mesh(const std::vector<float> &positions, const std::vector<float> &uvs)
{
    glGenVertexArrays(1, &vao_);
    glBindVertexArray(vao_);

    // Vertices
    glGenBuffers(1, &vertexBuffer_);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer_);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * positions.size(), positions.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(0);

    // Texture coordinates
    glGenBuffers(1, &uvBuffer_);
    glBindBuffer(GL_ARRAY_BUFFER, uvBuffer_);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * uvs.size(), uvs.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(1);

    verticesCount_ = positions.size() / 3;
}

Mesh::~Mesh()
{
    glDeleteVertexArrays(1, &vao_);
    glDeleteBuffers(1, &vertexBuffer_);
    glDeleteBuffers(1, &uvBuffer_);
}

void Mesh::draw()
{
    glBindVertexArray(vao_);
    glDrawArrays(GL_TRIANGLES, 0, verticesCount_);
}

auto Mesh::quad() -> std::shared_ptr<Mesh>
{
    static std::vector<float> positions = {
        -1, -1, 0,
        -1, 1, 0,
        1, 1, 0,
        -1, -1, 0,
        1, 1, 0,
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
    return nullptr;
}
