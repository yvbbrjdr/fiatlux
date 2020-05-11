#pragma once

#include <memory>
#include <string>
#include <vector>

#include "tiny_gltf.h"

#include "accel/bvh.h"
#include "bsdf.h"
#include "camera.h"
#include "common/mat4.h"
#include "pathtracer/intersection.h"
#include "pathtracer/ray.h"
#include "texture.h"
#include "triangle.h"

class Scene {
private:
    double aspect_ratio;
    tinygltf::Model model;
    std::unique_ptr<Camera> camera;
    std::vector<std::shared_ptr<Triangle>> triangles;
    std::vector<std::shared_ptr<Texture>> textures;
    std::vector<std::shared_ptr<BSDF>> bsdfs;
    std::unique_ptr<BVH> bvh;
    void load_node(const tinygltf::Node &node, const Mat4 &transform = Mat4());
    void load_camera(const tinygltf::Camera &camera, const Mat4 &transform);
    void load_mesh(const tinygltf::Mesh &mesh, const Mat4 &transform);
    std::shared_ptr<Texture> load_texture(size_t index);
    std::shared_ptr<BSDF> load_material(size_t index);

    template<class T1, class T2 = T1>
    std::vector<T2> load_buffer(const tinygltf::Accessor &accessor) const
    {
        auto &buffer_view = model.bufferViews[accessor.bufferView];
        auto &buffer = model.buffers[buffer_view.buffer];
        size_t n;
        switch (accessor.type) {
        case TINYGLTF_TYPE_SCALAR: n = 1;  break;
        case TINYGLTF_TYPE_VEC2:   n = 2;  break;
        case TINYGLTF_TYPE_VEC3:   n = 3;  break;
        case TINYGLTF_TYPE_VEC4:   n = 4;  break;
        case TINYGLTF_TYPE_MAT2:   n = 4;  break;
        case TINYGLTF_TYPE_MAT3:   n = 9;  break;
        case TINYGLTF_TYPE_MAT4:   n = 16; break;
        default:
            throw std::runtime_error("unsupported accessor type: " + std::to_string(accessor.type));
            break;
        }
        size_t start = buffer_view.byteOffset + accessor.byteOffset;
        size_t end = buffer_view.byteOffset + buffer_view.byteLength;
        size_t stride = buffer_view.byteStride ? buffer_view.byteStride : sizeof(T1) * n;
        size_t count = accessor.count;
        std::vector<T2> ret;
        for (size_t i = start, j = 0; i < end && j < count; i += stride, ++j)
            for (size_t j = 0; j < n; ++j)
                ret.push_back(*reinterpret_cast<const T1 *>(&buffer.data[i + j * sizeof(T1)]));
        return ret;
    }
public:
    std::vector<std::shared_ptr<Triangle>> lights;
    Scene(double aspect_ratio);
    void load(const std::string &filename);
    Ray generate_ray(double x, double y) const;
    bool intersect(Ray &r, Intersection &isect) const;
};
