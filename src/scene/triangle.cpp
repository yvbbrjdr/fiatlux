#include "triangle.h"

using namespace std;

Triangle::Triangle(const Vec3 &v0, const Vec3 &v1, const Vec3 &v2,
                   const Vec3 &n0, const Vec3 &n1, const Vec3 &n2,
                   const Vec3 &uv0, const Vec3 &uv1, const Vec3 &uv2,
                   shared_ptr<BSDF> bsdf) :
    v0(v0), v1(v1), v2(v2), n0(n0), n1(n1), n2(n2), uv0(uv0), uv1(uv1), uv2(uv2),
    area((v1 - v0).det(v2 - v0).norm() / 2), bsdf(bsdf) {}

bool Triangle::intersect(Ray &r, Intersection &isect) const
{
    Vec3 e1 = v1 - v0;
    Vec3 e2 = v2 - v0;
    Vec3 s = r.o - v0;
    Vec3 s1 = r.d.det(e2);
    Vec3 s2 = s.det(e1);
    Vec3 res(s2.dot(e2), s1.dot(s), s2.dot(r.d));
    res /= s1.dot(e1);
    double t = res.x;
    double b1 = res.y;
    double b2 = res.z;
    double b0 = 1 - b1 - b2;
    bool ret = b0 >= 0 && b1 >= 0 && b2 >= 0 && r.in_range(t);
    if (ret) {
        r.max_t = t;
        isect.t = t;
        isect.n = (n0 * b0 + n1 * b1 + n2 * b2).normalize();
        isect.uv = uv0 * b0 + uv1 * b1 + uv2 * b2;
        isect.bsdf = bsdf;
    }
    return ret;
}

Vec3 Triangle::sample(Sampler2D &sampler, const Vec3 &p, Vec3 &in, double &dist, double &pdf) const
{
    Vec3 u = sampler.sample(pdf);
    double su0 = sqrt(u.x);
    double b0 = 1 - su0;
    double b1 = u.y * su0;
    Vec3 sample = v0 * b0 + v1 * b1 + v2 * (1 - b0 - b1);
    Vec3 normal = (n0 * b0 + n1 * b1 + n2 * (1 - b0 - b1)).normalize();
    Vec3 d = sample - p;
    if (d.dot(normal) >= 0) {
        pdf = 0;
        return Vec3();
    }
    double norm2 = d.norm2();
    dist = sqrt(norm2);
    in = d.normalize();
    pdf *= norm2 / (area * fabs(in.dot(normal)));
    return bsdf->emmision;
}

ostream &operator<<(ostream &os, const Triangle &t)
{
    return os << "Triangle(\n\t" << t.v0 << ",\n\t" << t.v1 << ",\n\t" << t.v2 << ",\n\t" <<
                                    t.n0 << ",\n\t" << t.n1 << ",\n\t" << t.n2 << ",\n\t" <<
                                    t.uv0 << ",\n\t" << t.uv1 << ",\n\t" << t.uv2 << ",\n\t&" <<
                                    *t.bsdf << "\n)";
}
