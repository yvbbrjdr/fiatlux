#include "bvh.h"

using namespace std;

BVH::BVH(const vector<shared_ptr<Triangle>> &triangles)
{
    for (auto &triangle : triangles)
        bbox.extend(*triangle);
    if (triangles.size() <= 5) {
        this->triangles = triangles;
        return;
    }
    Vec3 midp = (bbox.minp + bbox.maxp) / 2;
    Vec3 diagonal = bbox.maxp - bbox.minp;
    vector<shared_ptr<Triangle>> left, right;
    if (diagonal.x >= diagonal.y && diagonal.x >= diagonal.z)
        for (auto &triangle : triangles) {
            BBox bbox_triangle;
            bbox_triangle.extend(*triangle);
            if ((bbox_triangle.minp.x + bbox_triangle.maxp.x) / 2 <= midp.x)
                left.push_back(triangle);
            else
                right.push_back(triangle);
        }
    else if (diagonal.y >= diagonal.x && diagonal.y >= diagonal.z)
        for (auto &triangle : triangles) {
            BBox bbox_triangle;
            bbox_triangle.extend(*triangle);
            if ((bbox_triangle.minp.y + bbox_triangle.maxp.y) / 2 <= midp.y)
                left.push_back(triangle);
            else
                right.push_back(triangle);
        }
    else
        for (auto &triangle : triangles) {
            BBox bbox_triangle;
            bbox_triangle.extend(*triangle);
            if ((bbox_triangle.minp.z + bbox_triangle.maxp.z) / 2 <= midp.z)
                left.push_back(triangle);
            else
                right.push_back(triangle);
        }
    if (left.size() == 0 || right.size() == 0) {
        left.clear();
        right.clear();
        for (size_t i = 0; i < triangles.size() / 2; ++i)
            left.push_back(triangles[i]);
        for (size_t i = triangles.size() / 2; i < triangles.size(); ++i)
            right.push_back(triangles[i]);
    }
    l_child.reset(new BVH(left));
    r_child.reset(new BVH(right));
}

bool BVH::intersect(Ray &r, Intersection &isect) const
{
    if (!bbox.intersect(r))
        return false;
    if (triangles.size() > 0) {
        bool ret = false;
        for (auto &triangle : triangles)
            ret |= triangle->intersect(r, isect);
        return ret;
    }
    bool ret = l_child->intersect(r, isect);
    ret |= r_child->intersect(r, isect);
    return ret;
}
