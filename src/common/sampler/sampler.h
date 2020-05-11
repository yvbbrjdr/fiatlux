#pragma once

#include "common/random.h"
#include "common/vec3.h"

class Sampler {
protected:
    Random r;
public:
    virtual Vec3 sample(double &pdf) = 0;
    virtual ~Sampler();
};
