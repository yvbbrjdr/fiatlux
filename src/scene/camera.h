#pragma once

#include "common/mat4.h"
#include "pathtracer/ray.h"

class Camera {
private:
    Vec3 position;
    Mat4 transform;
    Mat4 rotation;
    double xfov, yfov;
    double xspan, yspan;
    double znear, zfar;
public:
    Camera(const Mat4 &transform, double aspect_ratio, double yfov, double znear, double zfar);
    Ray generate_ray(double x, double y) const;
    friend std::ostream &operator<<(std::ostream &os, const Camera &c);
};
