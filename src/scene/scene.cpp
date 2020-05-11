#include "scene.h"

#include <iostream>

using namespace std;

void Scene::load_node(const tinygltf::Node &node, const Mat4 &transform)
{
    Mat4 cur_transform = [&] {
        if (!node.matrix.empty())
            return Mat4(node.matrix);
        Mat4 translation = [&] {
            if (node.translation.empty())
                return Mat4();
            return Mat4({
                1, 0, 0, node.translation[0],
                0, 1, 0, node.translation[1],
                0, 0, 1, node.translation[2],
                0, 0, 0, 1
            });
        }();
        Mat4 rotation = [&] {
            if (node.rotation.empty())
                return Mat4();
            double qi = node.rotation[0];
            double qj = node.rotation[1];
            double qk = node.rotation[2];
            double qr = node.rotation[3];
            return Mat4({
                1 - 2 * (qj * qj + qk * qk), 2 * (qi * qj - qk * qr), 2 * (qi * qk + qj * qr), 0,
                2 * (qi * qj + qk * qr), 1 - 2 * (qi * qi + qk * qk), 2 * (qj * qk - qi * qr), 0,
                2 * (qi * qk - qj * qr), 2 * (qj * qk + qi * qr), 1 - 2 * (qi * qi + qj * qj), 0,
                0, 0, 0, 1
            });
        }();
        Mat4 scale = [&] {
            if (node.scale.empty())
                return Mat4();
            return Mat4({
                node.scale[0], 0, 0, 0,
                0, node.scale[1], 0, 0,
                0, 0, node.scale[2], 0,
                0, 0, 0, 1
            });
        }();
        return translation * rotation * scale;
    }();
    cur_transform = transform * cur_transform;
    if (node.camera != -1)
        load_camera(model.cameras[node.camera], cur_transform);
    else if (node.mesh != -1)
        load_mesh(model.meshes[node.mesh], cur_transform);
    for (auto &index : node.children)
        load_node(model.nodes[index], cur_transform);
}

void Scene::load_camera(const tinygltf::Camera &camera, const Mat4 &transform)
{
    if (this->camera) {
        cerr << "Multiple cameras found in the scene. The renderer will use the first one." << endl;
        return;
    }
    if (camera.type != "perspective")
        throw runtime_error("unsupported camera type: " + camera.type);
    auto &p = camera.perspective;
    this->camera.reset(new Camera(transform, aspect_ratio, p.yfov, p.znear, p.zfar));
}

void Scene::load_mesh(const tinygltf::Mesh &mesh, const Mat4 &transform)
{
    for (auto &primitive : mesh.primitives) {
        if (primitive.mode != TINYGLTF_MODE_TRIANGLES)
            throw runtime_error("unsupported primitive mode: " + to_string(primitive.mode));
        const auto &position_iter = primitive.attributes.find("POSITION");
        const auto &normal_iter = primitive.attributes.find("NORMAL");
        const auto &uv_iter = primitive.attributes.find("TEXCOORD_0");
        if (position_iter == primitive.attributes.end())
            throw runtime_error("POSITION attribute not found");
        if (normal_iter == primitive.attributes.end())
            throw runtime_error("NORMAL attribute not found");
        bool has_uv = uv_iter != primitive.attributes.end();
        auto position_flat = load_buffer<float>(model.accessors[position_iter->second]);
        vector<Vec3> position;
        for (size_t i = 0; i < position_flat.size(); i += 3) {
            Vec3 tmp(position_flat[i], position_flat[i + 1], position_flat[i + 2]);
            position.push_back(transform * tmp);
        }
        auto normal_flat = load_buffer<float>(model.accessors[normal_iter->second]);
        vector<Vec3> normal;
        for (size_t i = 0; i < normal_flat.size(); i += 3) {
            Vec3 tmp(normal_flat[i], normal_flat[i + 1], normal_flat[i + 2]);
            normal.push_back((transform.translation_stripped() * tmp).normalize());
        }
        vector<Vec3> uv;
        if (has_uv) {
            auto uv_flat = load_buffer<float>(model.accessors[uv_iter->second]);
            for (size_t i = 0; i < uv_flat.size(); i += 2)
                uv.emplace_back(uv_flat[i], uv_flat[i + 1], 0);
        } else {
            uv.resize(position.size());
        }
        auto &index_type = model.accessors[primitive.indices].componentType;
        vector<uint32_t> indices;
        if (index_type == TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT)
            indices = load_buffer<uint16_t, uint32_t>(model.accessors[primitive.indices]);
        else
            indices = load_buffer<uint32_t>(model.accessors[primitive.indices]);
        auto bsdf = load_material(primitive.material);
        for (size_t i = 0; i < indices.size(); i += 3) {
            auto tmp = make_shared<Triangle>(position[indices[i]], position[indices[i + 1]], position[indices[i + 2]],
                                             normal[indices[i]], normal[indices[i + 1]], normal[indices[i + 2]],
                                             uv[indices[i]], uv[indices[i + 1]], uv[indices[i + 2]],
                                             bsdf);
            triangles.push_back(tmp);
            if (bsdf->is_light())
                lights.push_back(tmp);
        }
    }
}

shared_ptr<Texture> Scene::load_texture(size_t index)
{
    const static double correction = 2.2;
    if (textures[index])
        return textures[index];
    auto &t = model.textures[index];
    auto &image_flat = model.images[t.source];
    vector<Vec3> image;
    for (size_t i = 0; i < image_flat.image.size(); i += 4)
        image.emplace_back(pow(image_flat.image[i + 0] / 255.0, correction),
                           pow(image_flat.image[i + 1] / 255.0, correction),
                           pow(image_flat.image[i + 2] / 255.0, correction));
    textures[index] = make_shared<Texture>(image_flat.width, image_flat.height, image);
    return textures[index];
}

shared_ptr<BSDF> Scene::load_material(size_t index)
{
    if (bsdfs[index])
        return bsdfs[index];
    auto &m = model.materials[index];
    auto &pbr = m.pbrMetallicRoughness;
    shared_ptr<Texture> texture;
    if (pbr.baseColorTexture.index >= 0)
        texture = load_texture(pbr.baseColorTexture.index);
    bsdfs[index] = make_shared<BSDF>(pbr.baseColorFactor, texture, pbr.metallicFactor, pbr.roughnessFactor, m.emissiveFactor);
    return bsdfs[index];
}

Scene::Scene(double aspect_ratio) : aspect_ratio(aspect_ratio) {}

void Scene::load(const string &filename)
{
    tinygltf::TinyGLTF loader;
    string err, warn;
    size_t last_dot = filename.find_last_of(".");
    string ext = last_dot == string::npos ? "" : filename.substr(last_dot + 1);
    bool ret;
    cout << "Loading scene. . . " << flush;
    if (ext == "glb")
        ret = loader.LoadBinaryFromFile(&model, &err, &warn, filename);
    else
        ret = loader.LoadASCIIFromFile(&model, &err, &warn, filename);
    if (!warn.empty())
        cerr << warn << endl;
    if (!err.empty())
        throw runtime_error(err);
    if (!ret)
        throw runtime_error("failed to parse glTF");
    if (model.defaultScene == -1)
        throw runtime_error("default scene not found");
    bsdfs.resize(model.materials.size());
    textures.resize(model.textures.size());
    for (auto &index : model.scenes[model.defaultScene].nodes)
        load_node(model.nodes[index]);
    if (!camera)
        throw runtime_error("camera not found in the scene");
    cout << "Done!" << endl;
    cout << triangles.size() << " triangle(s) loaded." << endl;
    cout << lights.size() << " light(s) loaded." << endl;
    cout << "Building BVH. . . " << flush;
    bvh.reset(new BVH(triangles));
    triangles.clear();
    triangles.shrink_to_fit();
    textures.clear();
    textures.shrink_to_fit();
    bsdfs.clear();
    bsdfs.shrink_to_fit();
    cout << "Done!" << endl;
}

Ray Scene::generate_ray(double x, double y) const
{
    if (!camera)
        return Ray(Vec3(), Vec3(), 0, 0);
    return camera->generate_ray(x, y);
}

bool Scene::intersect(Ray &r, Intersection &isect) const
{
    if (!bvh)
        return false;
    return bvh->intersect(r, isect);
}
