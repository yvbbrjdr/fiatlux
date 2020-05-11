#pragma once

#include <functional>
#include <string>
#include <vector>

#include <SDL2/SDL.h>

#include "common/vec3.h"

class GUI {
private:
    static bool inited;
    static size_t obj_count;
    size_t width;
    SDL_Window *window;
    static void init();
    static void destroy();
    void set_pixel(size_t x, size_t y, uint8_t r, uint8_t g, uint8_t b) const;
public:
    typedef std::function<void(int32_t)> KeydownCallback;
    GUI(const std::string &title, size_t width, size_t height);
    void clear() const;
    void set_title(const std::string &title) const;
    void set_image(const std::vector<Vec3> &image, size_t x_start, size_t x_end, size_t y_start, size_t y_end) const;
    void run(KeydownCallback keydown_cb = KeydownCallback()) const;
    ~GUI();
};
