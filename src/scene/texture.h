#pragma once

#include <vector>

#include "common/vec3.h"

class Texture {
private:
    size_t width, height;
    std::vector<Vec3> image;

    template <class T>
    inline static T lerp(double t, const T &a, const T &b)
    {
        return (b - a) * t + a;
    }

    template <class T>
    inline static T bilerp(double t1, double t2, const T &a, const T &b, const T &c, const T &d)
    {
        return lerp(t1, lerp(t2, a, b), lerp(t2, c, d));
    }
public:
    Texture(size_t width, size_t height, const std::vector<Vec3> &image);
    Vec3 sample(const Vec3 &uv) const;
};
