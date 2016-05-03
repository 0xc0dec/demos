#include "Matrix.h"
#include "Math.h"
#include <memory>


const size_t MatrixSize = sizeof(float) * 16;


Matrix::Matrix()
{
    *this = identity();
}


Matrix::Matrix(float m11, float m12, float m13, float m14, float m21, float m22, float m23, float m24,
               float m31, float m32, float m33, float m34, float m41, float m42, float m43, float m44)
{
    m[0] = m11;
    m[1] = m21;
    m[2] = m31;
    m[3] = m41;
    m[4] = m12;
    m[5] = m22;
    m[6] = m32;
    m[7] = m42;
    m[8] = m13;
    m[9] = m23;
    m[10] = m33;
    m[11] = m43;
    m[12] = m14;
    m[13] = m24;
    m[14] = m34;
    m[15] = m44;
}


auto Matrix::identity() -> Matrix
{
    static Matrix m(
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1);
    return m;
}


auto Matrix::createPerspective(float fieldOfView, float aspectRatio, float znear, float zfar) -> Matrix
{
    auto f_n = 1.0f / (zfar - znear);
    auto theta = Math::degToRad(fieldOfView) * 0.5f;

    auto divisor = tan(theta);
    auto factor = 1.0f / divisor;

    Matrix result;
    memset(&result.m, 0, MatrixSize);
    result.m[0] = (1.0f / aspectRatio) * factor;
    result.m[5] = factor;
    result.m[10] = -(zfar + znear) * f_n;
    result.m[11] = -1.0f;
    result.m[14] = -2.0f * zfar * znear * f_n;

    return result;
}


auto Matrix::createTranslation(const Vector3& translation) -> Matrix
{
    Matrix result;
    result.m[12] = translation.x;
    result.m[13] = translation.y;
    result.m[14] = translation.z;
    return result;
}


auto Matrix::createScale(const Vector3& scale) -> Matrix
{
    Matrix result;
    result.m[0] = scale.x;
    result.m[5] = scale.y;
    result.m[10] = scale.z;
    return result;
}


auto Matrix::createRotationY(float radianAngle) -> Matrix
{
    auto c = cos(radianAngle);
    auto s = sin(radianAngle);

    Matrix result;

    result.m[0] = c;
    result.m[2] = -s;
    result.m[8] = s;
    result.m[10] = c;

    return result;
}


void Matrix::translate(const Vector3& t)
{
    auto tm = createTranslation(t);
    *this *= tm;
}


void Matrix::scaleByVector(const Vector3& s)
{
    auto sm = createScale(s);
    *this *= sm;
}


void Matrix::rotateY(float radianAngle)
{
    auto r = createRotationY(radianAngle);
    *this *= r;
}


auto Matrix::operator*=(const Matrix& m2) -> Matrix&
{
    float product[16];

    product[0] = m[0] * m2.m[0] + m[4] * m2.m[1] + m[8] * m2.m[2] + m[12] * m2.m[3];
    product[1] = m[1] * m2.m[0] + m[5] * m2.m[1] + m[9] * m2.m[2] + m[13] * m2.m[3];
    product[2] = m[2] * m2.m[0] + m[6] * m2.m[1] + m[10] * m2.m[2] + m[14] * m2.m[3];
    product[3] = m[3] * m2.m[0] + m[7] * m2.m[1] + m[11] * m2.m[2] + m[15] * m2.m[3];

    product[4] = m[0] * m2.m[4] + m[4] * m2.m[5] + m[8] * m2.m[6] + m[12] * m2.m[7];
    product[5] = m[1] * m2.m[4] + m[5] * m2.m[5] + m[9] * m2.m[6] + m[13] * m2.m[7];
    product[6] = m[2] * m2.m[4] + m[6] * m2.m[5] + m[10] * m2.m[6] + m[14] * m2.m[7];
    product[7] = m[3] * m2.m[4] + m[7] * m2.m[5] + m[11] * m2.m[6] + m[15] * m2.m[7];

    product[8] = m[0] * m2.m[8] + m[4] * m2.m[9] + m[8] * m2.m[10] + m[12] * m2.m[11];
    product[9] = m[1] * m2.m[8] + m[5] * m2.m[9] + m[9] * m2.m[10] + m[13] * m2.m[11];
    product[10] = m[2] * m2.m[8] + m[6] * m2.m[9] + m[10] * m2.m[10] + m[14] * m2.m[11];
    product[11] = m[3] * m2.m[8] + m[7] * m2.m[9] + m[11] * m2.m[10] + m[15] * m2.m[11];

    product[12] = m[0] * m2.m[12] + m[4] * m2.m[13] + m[8] * m2.m[14] + m[12] * m2.m[15];
    product[13] = m[1] * m2.m[12] + m[5] * m2.m[13] + m[9] * m2.m[14] + m[13] * m2.m[15];
    product[14] = m[2] * m2.m[12] + m[6] * m2.m[13] + m[10] * m2.m[14] + m[14] * m2.m[15];
    product[15] = m[3] * m2.m[12] + m[7] * m2.m[13] + m[11] * m2.m[14] + m[15] * m2.m[15];

    memcpy(m, product, MatrixSize);

    return *this;
}
