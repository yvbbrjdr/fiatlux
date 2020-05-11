#include "texture.h"

using namespace std;

Texture::Texture(size_t width, size_t height, const vector<Vec3> &image) : width(width), height(height), image(image) {}

Vec3 Texture::sample(const Vec3 &uv) const
{
    double x = uv.y * (height - 1);
    double y = uv.x * (width - 1);
    size_t i = x;
    size_t j = y;
    x -= i;
    y -= j;
    return bilerp(x, y, image[i * width + j], image[i * width + j + 1],
                        image[(i + 1) * width + j], image[(i + 1) * width + j + 1]);
}
