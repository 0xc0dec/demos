/*
    Copyright (c) Aleksey Fedotov
    MIT license
*/

#pragma once

#include <memory>
#include <vector>
#include <GL/glew.h>

namespace gl
{
    class Mesh
    {
    public:
        static auto quad()->std::shared_ptr<Mesh>;
        static auto box()->std::shared_ptr<Mesh>;

        // Build from vertex positions and texture coordinates
        Mesh(const std::vector<float> &positions, const std::vector<float> &uvs);

        ~Mesh();

        void draw() const;

    private:
        int32_t verticesCount_ = 0;
        GLuint vao_ = 0;
        std::vector<GLuint> buffers_;
    };
}
