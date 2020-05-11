#include "png.h"

#include "lodepng.h"

using namespace std;

void PNG::encode_bytes(const vector<uint8_t> &data, size_t width, const string &filename) const
{
    unsigned err = lodepng::encode(filename, data, width, data.size() / width / 3, LCT_RGB);
    if (err != 0)
        throw runtime_error(lodepng_error_text(err));
}
