#include <atomic>
#include <iostream>
#include <thread>

#include "gui/gui.h"
#include "image/png.h"
#include "pathtracer/pathtracer.h"
#include "scene/scene.h"

using namespace std;

int main(int argc, char *argv[])
{
    try {
        if (argc != 4) {
            cerr << "usage: " << argv[0] << " scene width height" << endl;
            return EXIT_FAILURE;
        }
        size_t width = atoi(argv[2]);
        size_t height = atoi(argv[3]);
        size_t spp = 1;
        size_t shadow_rays = 1;
        size_t depth = 8;
        Scene s(static_cast<double>(width) / height);
        s.load(argv[1]);
        thread render_thread;
        atomic_bool rendering(false);
        GUI gui("fiatlux", width, height);
        gui.run([&](int32_t key) {
            switch (key) {
            case 'r':
                if (rendering)
                    rendering = false;
                if (render_thread.joinable())
                    render_thread.join();
                gui.set_title("fiatlux");
                gui.clear();
                rendering = true;
                render_thread = thread([&]() {
                    auto image = Pathtracer(width, height, spp, shadow_rays, depth, 64)
                        .render(s, rendering, [&](const vector<Vec3> &image, size_t blocks_rendered, size_t block_count,
                                                  size_t x_start, size_t x_end, size_t y_start, size_t y_end) {
                        gui.set_title("fiatlux (" + to_string(blocks_rendered) + '/' + to_string(block_count) + ')');
                        gui.set_image(image, x_start, x_end, y_start, y_end);
                    });
                    PNG().encode(image, width, "output.png");
                    rendering = false;
                });
                break;
            case 'q':
                spp *= 2;
                cout << "spp: " << spp << endl;
                break;
            case 'a':
                if (spp > 1)
                    spp /= 2;
                cout << "spp: " << spp << endl;
                break;
            case 'w':
                shadow_rays *= 2;
                cout << "shadow_rays: " << shadow_rays << endl;
                break;
            case 's':
                if (shadow_rays > 1)
                    shadow_rays /= 2;
                cout << "shadow_rays: " << shadow_rays << endl;
                break;
            case 'e':
                ++depth;
                cout << "depth: " << depth << endl;
                break;
            case 'd':
                if (depth > 0)
                    --depth;
                cout << "depth: " << depth << endl;
                break;
            }
        });
        if (rendering)
            rendering = false;
        if (render_thread.joinable())
            render_thread.join();
        return EXIT_SUCCESS;
    } catch (const exception &e) {
        cerr << "fatal: " << e.what() << endl;
        return EXIT_FAILURE;
    }
}
