#pragma once

#include <memory>

#include "common/sampler/sampler_hemisphere.h"
#include "common/vec3.h"
#include "texture.h"

class BSDF {
private:
    Vec3 base_color;
    std::shared_ptr<Texture> texture;
    double metallic;
    double roughness;
    bool is_light_cache;
public:
    Vec3 emmision;
    BSDF(const Vec3 &base_color, std::shared_ptr<Texture> texture,
         double metallic, double roughness, const Vec3 &emmision);
    bool is_light() const;
    Vec3 f(const Vec3 &in, const Vec3 &out, const Vec3 &uv) const;
    Vec3 sample(SamplerHemisphere &sampler, Vec3 &in, const Vec3 &out, const Vec3 &uv, double &pdf) const;
    friend std::ostream &operator<<(std::ostream &os, const BSDF &bsdf);
};
