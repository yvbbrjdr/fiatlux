#include "sampler_2d.h"

Vec3 Sampler2D::sample(double &pdf)
{
    pdf = 1;
    return Vec3(r.uniform(), r.uniform(), 0);
}
