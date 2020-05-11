#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include "common/vec3.h"

class Image {
protected:
    virtual void encode_bytes(const std::vector<uint8_t> &data, size_t width, const std::string &filename) const = 0;
public:
    static std::vector<uint8_t> to_bytes(const std::vector<Vec3> &data);
    void encode(const std::vector<Vec3> &data, size_t width, const std::string &filename) const;
    virtual ~Image();
};
