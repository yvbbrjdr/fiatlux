#include "image.h"

#include <cmath>

using namespace std;

vector<uint8_t> Image::to_bytes(const vector<Vec3> &data)
{
    const static double exposure = pow(2, 1.2);
    const static double correction = 1 / 2.2;
    vector<uint8_t> ret(data.size() * 3, 0);
    for (size_t i = 0; i < data.size(); ++i) {
        double r = data[i].x * exposure;
        double g = data[i].y * exposure;
        double b = data[i].z * exposure;
        r = r < 0.0 ? 0.0 : r;
        r = r > 1.0 ? 1.0 : r;
        g = g < 0.0 ? 0.0 : g;
        g = g > 1.0 ? 1.0 : g;
        b = b < 0.0 ? 0.0 : b;
        b = b > 1.0 ? 1.0 : b;
        ret[3 * i + 0] = pow(r, correction) * 255.999;
        ret[3 * i + 1] = pow(g, correction) * 255.999;
        ret[3 * i + 2] = pow(b, correction) * 255.999;
    }
    return ret;
}

void Image::encode(const vector<Vec3> &data, size_t width, const string &filename) const
{
    encode_bytes(to_bytes(data), width, filename);
}

Image::~Image() {}
