#pragma once

#include <cmath>
#include <ostream>
#include <vector>

class Vec3 {
public:
    double x, y, z;

    inline Vec3() : Vec3(0) {}

    inline Vec3(double x, double y, double z) : x(x), y(y), z(z) {}

    inline Vec3(double a) : Vec3(a, a, a) {}

    inline Vec3(const std::vector<double> &v) : Vec3(v[0], v[1], v[2]) {}

    inline Vec3 operator+(const Vec3 &other) const
    {
        return Vec3(x + other.x, y + other.y, z + other.z);
    }

    inline Vec3 &operator+=(const Vec3 &other)
    {
        x += other.x;
        y += other.y;
        z += other.z;
        return *this;
    }

    inline Vec3 operator-() const
    {
        return Vec3(-x, -y, -z);
    }

    inline Vec3 operator-(const Vec3 &other) const
    {
        return Vec3(x - other.x, y - other.y, z - other.z);
    }

    inline Vec3 &operator-=(const Vec3 &other)
    {
        x -= other.x;
        y -= other.y;
        z -= other.z;
        return *this;
    }

    inline Vec3 operator*(const Vec3 &other) const
    {
        return Vec3(x * other.x, y * other.y, z * other.z);
    }

    inline Vec3 &operator*=(const Vec3 &other)
    {
        x *= other.x;
        y *= other.y;
        z *= other.z;
        return *this;
    }

    inline Vec3 operator/(const Vec3 &other) const
    {
        return Vec3(x / other.x, y / other.y, z / other.z);
    }

    inline Vec3 &operator/=(const Vec3 &other)
    {
        x /= other.x;
        y /= other.y;
        z /= other.z;
        return *this;
    }

    inline Vec3 operator*(const double &other) const
    {
        return Vec3(x * other, y * other, z * other);
    }

    inline Vec3 &operator*=(const double &other)
    {
        x *= other;
        y *= other;
        z *= other;
        return *this;
    }

    inline Vec3 operator/(const double &other) const
    {
        double inv = 1 / other;
        return Vec3(x * inv, y * inv, z * inv);
    }

    inline Vec3 &operator/=(const double &other)
    {
        return *this *= 1 / other;
    }

    inline double dot(const Vec3 &other) const
    {
        return x * other.x + y * other.y + z * other.z;
    }

    inline Vec3 det(const Vec3 &other) const
    {
        return Vec3(y * other.z - z * other.y, z * other.x - x * other.z, x * other.y - y * other.x);
    }

    inline double norm() const
    {
        return sqrt(norm2());
    }

    inline double norm2() const
    {
        return dot(*this);
    }

    inline Vec3 normalized() const
    {
        double n = norm();
        return n == 0.0 ? Vec3() : *this / n;
    }

    inline Vec3 &normalize()
    {
        return *this = normalized();
    }

    inline double illuminance() const
    {
        return 0.2126 * x + 0.7152 * y + 0.0722 * z;
    }

    inline friend std::ostream &operator<<(std::ostream &os, const Vec3 &v)
    {
        return os << "Vec3(" << v.x << ", " << v.y << ", " << v.z << ')';
    }
};
