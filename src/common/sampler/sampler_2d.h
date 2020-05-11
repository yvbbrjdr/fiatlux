#pragma once

#include "sampler.h"

class Sampler2D : public Sampler {
public:
    Vec3 sample(double &pdf);
};
