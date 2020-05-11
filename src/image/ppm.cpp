#include "ppm.h"

#include <fstream>

using namespace std;

void PPM::encode_bytes(const vector<uint8_t> &data, size_t width, const string &filename) const
{
    ofstream file(filename);
    if (!file)
        throw runtime_error("failed to open file for writing");
    file << "P3" << endl << width << ' ' << data.size() / width / 3 << endl << 255 << endl;
    for (size_t i = 0; i < data.size(); ++i)
        file << static_cast<uint32_t>(data[i]) << (i % 3 == 2 ? '\n' : ' ');
}
