#include "gui.h"

#include <stdexcept>

#include "image/image.h"

using namespace std;

bool GUI::inited = false;
size_t GUI::obj_count = 0;

void GUI::init()
{
    if (inited)
        return;
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
        throw runtime_error(string("SDL could not be initialized: ") + SDL_GetError());
    inited = true;
}

void GUI::destroy()
{
    SDL_Quit();
}

void GUI::set_pixel(size_t x, size_t y, uint8_t r, uint8_t g, uint8_t b) const
{
    SDL_Surface *surface = SDL_GetWindowSurface(window);
    uint8_t *target = static_cast<uint8_t *>(surface->pixels) + x * surface->pitch + y * 4;
    target[0] = b;
    target[1] = g;
    target[2] = r;
}

GUI::GUI(const string &title, size_t width, size_t height) : width(width), window(nullptr)
{
    init();
    window = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                              width, height, SDL_WINDOW_SHOWN);
    if (window == nullptr)
        throw runtime_error(string("window could not be created: ") + SDL_GetError());
    ++obj_count;
    clear();
}

void GUI::clear() const
{
    SDL_Surface *surface = SDL_GetWindowSurface(window);
    SDL_FillRect(surface, nullptr, 0);
}

void GUI::set_title(const string &title) const
{
    SDL_SetWindowTitle(window, title.c_str());
}

void GUI::set_image(const vector<Vec3> &image, size_t x_start, size_t x_end, size_t y_start, size_t y_end) const
{
    vector<Vec3> image_block;
    size_t block_width = y_end - y_start;
    for (size_t i = x_start; i < x_end; ++i)
        for (size_t j = y_start; j < y_end; ++j)
            image_block.push_back(image[i * width + j]);
    auto pixel = Image::to_bytes(image_block);
    for (size_t i = x_start; i < x_end; ++i)
        for (size_t j = y_start; j < y_end; ++j) {
            size_t base = ((i - x_start) * block_width + j - y_start) * 3;
            set_pixel(i, j, pixel[base + 0], pixel[base + 1], pixel[base + 2]);
        }
}

void GUI::run(KeydownCallback keydown_cb) const
{
    SDL_Event ev;
    for (;;) {
        if (SDL_PollEvent(&ev))
            switch (ev.type) {
            case SDL_KEYDOWN:
                if (keydown_cb)
                    keydown_cb(ev.key.keysym.sym);
                break;
            case SDL_QUIT:
                return;
            }
        else
            SDL_Delay(16);
        SDL_UpdateWindowSurface(window);
    }
}

GUI::~GUI()
{
    SDL_DestroyWindow(window);
    window = nullptr;
    --obj_count;
    if (obj_count == 0)
        destroy();
}
