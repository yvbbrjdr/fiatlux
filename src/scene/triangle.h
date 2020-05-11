#pragma once

#include <memory>

#include "bsdf.h"
#include "common/sampler/sampler_2d.h"
#include "common/vec3.h"
#include "pathtracer/intersection.h"
#include "pathtracer/ray.h"

class Triangle {
public:
    Vec3 v0, v1, v2;
    Vec3 n0, n1, n2;
    Vec3 uv0, uv1, uv2;
    double area;
    std::shared_ptr<BSDF> bsdf;
    Triangle(const Vec3 &v0, const Vec3 &v1, const Vec3 &v2,
             const Vec3 &n0, const Vec3 &n1, const Vec3 &n2,
             const Vec3 &uv0, const Vec3 &uv1, const Vec3 &uv2,
             std::shared_ptr<BSDF> bsdf);
    bool intersect(Ray &r, Intersection &isect) const;
    Vec3 sample(Sampler2D &sampler, const Vec3 &p, Vec3 &in, double &dist, double &pdf) const;
    friend std::ostream &operator<<(std::ostream &os, const Triangle &t);
};
