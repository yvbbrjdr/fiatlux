#pragma once

#include "common/vec3.h"
#include "pathtracer/ray.h"
#include "scene/triangle.h"

class BBox {
public:
    Vec3 minp, maxp;
    BBox();
    void extend(const Triangle &t);
    bool intersect(const Ray &r) const;
};
