#include "bbox.h"

#include <limits>

using namespace std;

BBox::BBox() : minp(numeric_limits<double>::infinity()), maxp(-numeric_limits<double>::infinity()) {}

void BBox::extend(const Triangle &t)
{
    minp.x = min(min(minp.x, t.v0.x), min(t.v1.x, t.v2.x));
    minp.y = min(min(minp.y, t.v0.y), min(t.v1.y, t.v2.y));
    minp.z = min(min(minp.z, t.v0.z), min(t.v1.z, t.v2.z));
    maxp.x = max(max(maxp.x, t.v0.x), max(t.v1.x, t.v2.x));
    maxp.y = max(max(maxp.y, t.v0.y), max(t.v1.y, t.v2.y));
    maxp.z = max(max(maxp.z, t.v0.z), max(t.v1.z, t.v2.z));
}

bool BBox::intersect(const Ray &r) const
{
    double t;
    Vec3 p;
    t = (minp.x - r.o.x) / r.d.x;
    p = r.at(t);
    if (minp.y < p.y && p.y < maxp.y && minp.z < p.z && p.z < maxp.z)
        return true;
    t = (maxp.x - r.o.x) / r.d.x;
    p = r.at(t);
    if (minp.y < p.y && p.y < maxp.y && minp.z < p.z && p.z < maxp.z)
        return true;
    t = (minp.y - r.o.y) / r.d.y;
    p = r.at(t);
    if (minp.x < p.x && p.x < maxp.x && minp.z < p.z && p.z < maxp.z)
        return true;
    t = (maxp.y - r.o.y) / r.d.y;
    p = r.at(t);
    if (minp.x < p.x && p.x < maxp.x && minp.z < p.z && p.z < maxp.z)
        return true;
    t = (minp.z - r.o.z) / r.d.z;
    p = r.at(t);
    if (minp.x < p.x && p.x < maxp.x && minp.y < p.y && p.y < maxp.y)
        return true;
    t = (maxp.z - r.o.z) / r.d.z;
    p = r.at(t);
    if (minp.x < p.x && p.x < maxp.x && minp.y < p.y && p.y < maxp.y)
        return true;
    return false;
}
