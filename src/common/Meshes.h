#pragma once

#include <GL/glew.h>

class QuadMesh
{
public:
    QuadMesh();
    ~QuadMesh();

    void draw();

private:
    GLuint vao_ = 0;
    GLuint vertexBuffer_ = 0;
    GLuint uvBuffer_ = 0;
};
