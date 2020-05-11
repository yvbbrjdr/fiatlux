#pragma once

#include "common/vec3.h"

class Ray {
public:
    Vec3 o, d;
    double min_t, max_t;
    size_t depth;

    inline Ray(const Vec3 &o, const Vec3 &d, double min_t, double max_t, size_t depth = 0) :
        o(o), d(d), min_t(min_t), max_t(max_t), depth(depth) {}

    inline bool in_range(double t) const
    {
        return min_t < t && t < max_t;
    }

    inline Vec3 at(double t) const
    {
        return o + d * t;
    }

    inline friend std::ostream &operator<<(std::ostream &os, const Ray &r)
    {
        return os << "Ray(" << r.o << ", " << r.d << ", " << r.min_t << ", " << r.max_t << ')';
    }
};
