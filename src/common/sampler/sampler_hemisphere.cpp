#include "sampler_hemisphere.h"

#include <cmath>

Vec3 SamplerHemisphere::sample(double &pdf)
{
    double u1 = r.uniform();
    double u2 = r.uniform();
    double su1 = sqrt(u1);
    double somu1 = sqrt(1 - u1);
    double theta = 2 * M_PI * u2;
    pdf = somu1 / M_PI;
    return Vec3(su1 * cos(theta), su1 * sin(theta), somu1);
}
