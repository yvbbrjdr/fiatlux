#pragma once

#include <atomic>
#include <functional>

#include "common/sampler/sampler_2d.h"
#include "common/sampler/sampler_hemisphere.h"
#include "ray.h"
#include "scene/scene.h"

class Pathtracer {
private:
    size_t width, height;
    size_t spp, shadow_rays;
    size_t depth;
    size_t block_size;
    Vec3 render_ray(const Scene &scene, Ray &r, Sampler2D &sampler_2d, SamplerHemisphere &sampler_hemisphere) const;
    static Vec3 zero_bounce(const Intersection &isect);
    Vec3 one_bounce(const Scene &scene, Ray &r, const Intersection &isect, Sampler2D &sampler_2d) const;
    Vec3 multiple_bounces(const Scene &scene, Ray &r, const Intersection &isect, Sampler2D &sampler_2d,
                          SamplerHemisphere &sampler_hemisphere) const;
public:
    typedef std::function<void(const std::vector<Vec3> &, size_t, size_t,
                               size_t, size_t, size_t, size_t)> ProgressCallback;
    Pathtracer(size_t width, size_t height, size_t spp, size_t shadow_rays, size_t depth, size_t block_size);
    std::vector<Vec3> render(const Scene &scene, const std::atomic_bool &rendering,
                             ProgressCallback progress_cb = ProgressCallback()) const;
};
