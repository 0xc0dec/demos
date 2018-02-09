#pragma once

#include "Vector3.h"

class Matrix final
{
public:
    // column-major order
    float m[16];

    Matrix();
    Matrix(float m11, float m12, float m13, float m14,
           float m21, float m22, float m23, float m24,
           float m31, float m32, float m33, float m34,
           float m41, float m42, float m43, float m44);

    static auto identity() -> Matrix;

    static auto createPerspective(float fieldOfView, float aspectRatio, float znear, float zfar) -> Matrix;
    static auto createTranslation(const Vector3& translation) -> Matrix;
    static auto createScale(const Vector3& scale) -> Matrix;
    static auto createRotationY(float radianAngle) -> Matrix;

    void translate(const Vector3& t);
    void scaleByVector(const Vector3& s);
    void rotateY(float radianAngle);

    auto operator*(const Matrix& m) const -> Matrix;
    auto operator*=(const Matrix& m) -> Matrix&;
};

inline auto Matrix::operator*(const Matrix& m) const -> Matrix
{
    auto result(*this);
    return result *= m;
}
