#pragma once

#include "common/vec3.h"
#include "scene/bsdf.h"

class Intersection {
public:
    double t;
    Vec3 n;
    Vec3 uv;
    std::shared_ptr<BSDF> bsdf;
};
