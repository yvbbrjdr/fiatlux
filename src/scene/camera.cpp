#include "camera.h"

#include <cmath>

using namespace std;

Camera::Camera(const Mat4 &transform, double aspect_ratio, double yfov, double znear, double zfar) :
    position(transform * Vec3()), transform(transform), rotation(transform.translation_stripped()),
    xfov(yfov * aspect_ratio), yfov(yfov), xspan(tan(xfov / 2)), yspan(tan(yfov / 2)), znear(znear), zfar(zfar) {}

Ray Camera::generate_ray(double x, double y) const
{
    x = x * 2 - 1;
    y = y * 2 - 1;
    return Ray(position, (rotation * Vec3(x * xspan, y * yspan, -1)).normalize(), znear, zfar);
}

ostream &operator<<(ostream &os, const Camera &c)
{
    return os << "Camera(" << c.transform << ", " << c.xfov / c.yfov << ", " << c.yfov << ", " <<
                              c.znear << ", " << c.zfar << ')';
}
