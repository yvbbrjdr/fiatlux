#pragma once

#include <memory>
#include <vector>

#include "bbox.h"
#include "pathtracer/intersection.h"
#include "scene/triangle.h"

class BVH {
private:
    BBox bbox;
    std::vector<std::shared_ptr<Triangle>> triangles;
    std::unique_ptr<BVH> l_child, r_child;
public:
    BVH(const std::vector<std::shared_ptr<Triangle>> &triangles);
    bool intersect(Ray &r, Intersection &isect) const;
};
