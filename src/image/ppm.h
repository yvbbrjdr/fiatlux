#pragma once

#include "image.h"

class PPM : public Image {
protected:
    void encode_bytes(const std::vector<uint8_t> &data, size_t width, const std::string &filename) const;
};
