#include "bsdf.h"

using namespace std;

BSDF::BSDF(const Vec3 &base_color, shared_ptr<Texture> texture,
           double metallic, double roughness, const Vec3 &emmision) :
    base_color(base_color), texture(texture), metallic(metallic), roughness(roughness),
    is_light_cache(emmision.norm2() > 0.0), emmision(emmision) {}

bool BSDF::is_light() const
{
    return is_light_cache;
}

Vec3 BSDF::f(const Vec3 &in, const Vec3 &out, const Vec3 &uv) const
{
    if (in.z * out.z <= 0)
        return Vec3();
    Vec3 color = base_color;
    if (texture)
        color *= texture->sample(uv);
    return color / M_PI;
}

Vec3 BSDF::sample(SamplerHemisphere &sampler, Vec3 &in, const Vec3 &out, const Vec3 &uv, double &pdf) const
{
    in = sampler.sample(pdf);
    if (out.z < 0)
        in.z = -in.z;
    return f(in, out, uv);
}

ostream &operator<<(ostream &os, const BSDF &bsdf)
{
    return os << "BSDF(" << bsdf.base_color << ", " << bsdf.metallic << ", " <<
                            bsdf.roughness  << ", " << bsdf.emmision << ')';
}
