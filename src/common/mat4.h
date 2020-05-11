#pragma once

#include <ostream>
#include <vector>

#include "vec3.h"

class Mat4 {
private:
    enum {
        WIDTH = 4
    };
public:
    double m[WIDTH][WIDTH];

    inline Mat4() : Mat4({1, 0, 0, 0,
                          0, 1, 0, 0,
                          0, 0, 1, 0,
                          0, 0, 0, 1}) {}

    inline Mat4(const std::vector<double> &v)
    {
        for (size_t i = 0; i < WIDTH; ++i)
            for (size_t j = 0; j < WIDTH; ++j)
                m[i][j] = v[i * WIDTH + j];
    }

    inline static Mat4 coord_space(const Vec3 &n)
    {
        const Vec3 z = n;
        Vec3 tmp = n;
        if (fabs(tmp.y) <= 1e-7)
            tmp.y = 1;
        else
            tmp.x += 1;
        Vec3 x = tmp.det(z).normalize();
        Vec3 y = z.det(x);
        return Mat4({
            x.x, x.y, x.z, 0,
            y.x, y.y, y.z, 0,
            z.x, z.y, z.z, 0,
            0,   0,   0,   1
        });
    }

    inline Mat4 operator*(const Mat4 &other) const
    {
        std::vector<double> ret(WIDTH * WIDTH);
        for (size_t i = 0; i < WIDTH; ++i)
            for (size_t j = 0; j < WIDTH; ++j)
                for (size_t k = 0; k < WIDTH; ++k)
                    ret[i * WIDTH + j] += m[i][k] * other.m[k][j];
        return ret;
    }

    inline Vec3 operator*(const Vec3 &other) const
    {
        double ret[WIDTH];
        for (size_t i = 0; i < WIDTH; ++i)
            ret[i] = m[i][0] * other.x + m[i][1] * other.y + m[i][2] * other.z + m[i][3];
        ret[3] = 1 / ret[3];
        return Vec3(ret[0] * ret[3], ret[1] * ret[3], ret[2] * ret[3]);
    }

    inline Mat4 translation_stripped() const
    {
        return Mat4({
            m[0][0], m[0][1], m[0][2], 0,
            m[1][0], m[1][1], m[1][2], 0,
            m[2][0], m[2][1], m[2][2], 0,
            0      , 0      , 0      , 1
        });
    }

    inline Mat4 T() const
    {
        return Mat4({
            m[0][0], m[1][0], m[2][0], m[3][0],
            m[0][1], m[1][1], m[2][1], m[3][1],
            m[0][2], m[1][2], m[2][2], m[3][2],
            m[0][3], m[1][3], m[2][3], m[3][3]
        });
    }

    inline friend std::ostream &operator<<(std::ostream &os, const Mat4 &v)
    {
        os << "Mat4(" << std::endl;
        for (size_t i = 0; i < WIDTH; ++i) {
            for (size_t j = 0; j < WIDTH; ++j) {
                os << '\t' << v.m[i][j];
                if (i != WIDTH - 1 || j != WIDTH - 1)
                    os << ',';
            }
            os << std::endl;
        }
        os << ')';
        return os;
    }
};
