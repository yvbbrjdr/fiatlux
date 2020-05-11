#include "pathtracer.h"

#include <condition_variable>
#include <iostream>
#include <limits>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

#include "intersection.h"

using namespace std;

Vec3 Pathtracer::render_ray(const Scene &scene, Ray &r, Sampler2D &sampler_2d,
    SamplerHemisphere &sampler_hemisphere) const
{
    Intersection isect;
    if (!scene.intersect(r, isect))
        return Vec3();
    if (depth == 0)
        return zero_bounce(isect);
    return zero_bounce(isect) + multiple_bounces(scene, r, isect, sampler_2d, sampler_hemisphere);
}

Vec3 Pathtracer::zero_bounce(const Intersection &isect)
{
    return isect.bsdf->emmision;
}

Vec3 Pathtracer::one_bounce(const Scene &scene, Ray &r, const Intersection &isect, Sampler2D &sampler_2d) const
{
    Mat4 w2o = Mat4::coord_space(isect.n);
    Vec3 pos = r.at(isect.t);
    size_t total_shadow_rays = scene.lights.size() * shadow_rays;
    Vec3 ret;
    for (auto &light : scene.lights)
        for (size_t i = 0; i < shadow_rays; ++i) {
            Vec3 d;
            double dist;
            double pdf;
            Vec3 emission = light->sample(sampler_2d, pos, d, dist, pdf);
            if (pdf == 0)
                continue;
            Vec3 in = w2o * d;
            Ray shadow_ray(pos, d, 1e-7, dist - 1e-7);
            Intersection isect2;
            if (!scene.intersect(shadow_ray, isect2))
                ret += isect.bsdf->f(in, w2o * -r.d, isect.uv) * fabs(in.z) * emission / pdf;
        }
    ret /= total_shadow_rays;
    return ret;
}

Vec3 Pathtracer::multiple_bounces(const Scene &scene, Ray &r, const Intersection &isect, Sampler2D &sampler_2d,
                                  SamplerHemisphere &sampler_hemisphere) const
{
    Mat4 w2o = Mat4::coord_space(isect.n);
    Mat4 o2w = w2o.T();
    Vec3 pos = r.at(isect.t);
    Vec3 ret = one_bounce(scene, r, isect, sampler_2d);
    if (r.depth < depth - 1) {
        Vec3 in;
        double pdf;
        Vec3 sample = isect.bsdf->sample(sampler_hemisphere, in, w2o * -r.d, isect.uv, pdf);
        if (pdf == 0)
            return ret;
        Ray next_ray(pos, o2w * in, 1e-7, numeric_limits<double>::infinity(), r.depth + 1);
        Intersection isect2;
        if (scene.intersect(next_ray, isect2))
            ret += multiple_bounces(scene, next_ray, isect2, sampler_2d, sampler_hemisphere) *
                   sample * fabs(in.z) / pdf;
    }
    return ret;
}

Pathtracer::Pathtracer(size_t width, size_t height, size_t spp, size_t shadow_rays, size_t depth, size_t block_size) :
    width(width), height(height), spp(spp), shadow_rays(shadow_rays), depth(depth), block_size(block_size) {}

vector<Vec3> Pathtracer::render(const Scene &scene, const atomic_bool &rendering, ProgressCallback progress_cb) const
{
    vector<Vec3> ret(width * height);
    size_t block_x_count = height / block_size + (height % block_size > 0);
    size_t block_y_count = width / block_size + (width % block_size > 0);
    size_t block_count = block_x_count * block_y_count;
    size_t blocks_rendered = 0;
    struct FinishedJob { size_t x_start, x_end, y_start, y_end; };
    queue<FinishedJob> finished_q;
    vector<thread> thread_pool;
    atomic_size_t next_to_render(0);
    mutex lock;
    condition_variable cv;
    size_t num_of_threads = thread::hardware_concurrency();
    if (num_of_threads == 0)
        num_of_threads = 1;
    cout << "Blocks rendered: 0/" << block_count << '\r' << flush;
    time_t t = time(nullptr);
    for (size_t i = 0; i < num_of_threads; ++i)
        thread_pool.emplace_back([&]() {
            for (;;) {
                size_t block_id = next_to_render++;
                if (block_id >= block_count)
                    break;
                Sampler2D sampler_2d;
                SamplerHemisphere sampler_hemisphere;
                size_t x_start = block_id / block_y_count * block_size;
                size_t x_end = x_start + block_size;
                if (x_end > height)
                    x_end = height;
                size_t y_start = block_id % block_y_count * block_size;
                size_t y_end = y_start + block_size;
                if (y_end > width)
                    y_end = width;
                for (size_t i = x_start; i < x_end && rendering; ++i)
                    for (size_t j = y_start; j < y_end; ++j) {
                        Vec3 pixel;
                        double s1 = 0;
                        double s2 = 0;
                        size_t k;
                        for (k = 0; k < spp; ++k) {
                            double _;
                            Vec3 u = sampler_2d.sample(_);
                            Ray r = scene.generate_ray((j + u.x) / width, (height - i - 1 + u.y) / height);
                            Vec3 sample = render_ray(scene, r, sampler_2d, sampler_hemisphere);
                            pixel += sample;
                            double illum = sample.illuminance();
                            s1 += illum;
                            s2 += illum * illum;
                            if (k % 64 == 63) {
                                size_t n = k + 1;
                                double mean = s1 / n;
                                double var = (s2 - s1 * s1 / n) / (n - 1);
                                double I = 1.96 * sqrt(var / n);
                                if (I <= 0.05 * mean) {
                                    ++k;
                                    break;
                                }
                            }
                        }
                        ret[i * width + j] = pixel / k;
                    }
                {
                    unique_lock<mutex> guard(lock);
                    finished_q.push(FinishedJob{x_start, x_end, y_start, y_end});
                    cv.notify_one();
                }
            }
        });
    while (blocks_rendered < block_count && rendering) {
        FinishedJob job;
        {
            unique_lock<mutex> guard(lock);
            cv.wait(guard, [&]() { return finished_q.size() > 0; });
            job = finished_q.front();
            finished_q.pop();
        }
        cout << "Blocks rendered: " << ++blocks_rendered << '/' << block_count << '\r' << flush;
        if (progress_cb)
            progress_cb(ret, blocks_rendered, block_count, job.x_start, job.x_end, job.y_start, job.y_end);
    }
    for (auto &t : thread_pool)
        t.join();
    cout << endl << "Rendering took " << time(nullptr) - t << " second(s)." << endl;
    return ret;
}
