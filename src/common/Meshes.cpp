#include "Meshes.h"

QuadMesh::QuadMesh()
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

    glGenVertexArrays(1, &vao_);
    glBindVertexArray(vao_);

    // Vertices
    glGenBuffers(1, &vertexBuffer_);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer_);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * 18, vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(0);

    // Texture coordinates
    glGenBuffers(1, &uvBuffer_);
    glBindBuffer(GL_ARRAY_BUFFER, uvBuffer_);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 2 * 12, uvs, GL_STATIC_DRAW);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(1);
}

QuadMesh::~QuadMesh()
{
    glDeleteVertexArrays(1, &vao_);
    glDeleteBuffers(1, &vertexBuffer_);
    glDeleteBuffers(1, &uvBuffer_);
}

void QuadMesh::draw()
{
    glBindVertexArray(vao_);
    glDrawArrays(GL_TRIANGLES, 0, 6); // 6 vertices
}
