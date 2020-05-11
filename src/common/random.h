#pragma once

#include <random>

class Random {
private:
    std::random_device rd;
    std::mt19937 gen;
    std::uniform_real_distribution<> dis;
public:
    inline Random() : gen(rd()) {}

    inline double uniform()
    {
        return dis(gen);
    }
};
