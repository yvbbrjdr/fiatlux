#pragma once

#include "sampler.h"

class SamplerHemisphere : public Sampler {
public:
    Vec3 sample(double &pdf);
};
