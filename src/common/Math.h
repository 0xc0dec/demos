#pragma once

#include <cmath>

class Math final
{
public:
    static constexpr float smallFloat1 = 0.000001f;
    static constexpr float smallFloat2 = 1.0e-37f;
    static constexpr float smallFloat3 = 2e-37f;

    static bool approxZero(float value, float tolerance);
    static bool approxEqual(float first, float second, float tolerance);

    static auto degToRad(float degrees) -> float;
};

inline bool Math::approxZero(float value, float tolerance)
{
    return fabs(value) <= tolerance;
}

inline bool Math::approxEqual(float first, float second, float tolerance)
{
    return fabs(first - second) <= tolerance;
}

inline auto Math::degToRad(float degrees) -> float
{
    return degrees * 0.0174532925f;
}
