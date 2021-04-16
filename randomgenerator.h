#pragma once
#include <random>

// Класс можно дополнить различными распределениями из
// https://en.cppreference.com/w/cpp/numeric/random

class RandomGenerator {
private:
    std::mt19937 gen;

public:
    RandomGenerator() {
        std::random_device rd;
        gen.seed(rd());
    }

    double d_udist(double a, double b) {
        std::uniform_real_distribution<double> range(a, b);
        return range(gen);
    }

    int int_udist(int a, int b) {
        std::uniform_int_distribution<int> range(a, b);
        return range(gen);
    }

    friend size_t random_choice(const std::vector<int>& v);
};

static RandomGenerator gen;

size_t random_choice(const std::vector<int>& v){
    double prob = gen.d_udist(0, 1);
    for (size_t i = 0; i != v.size(); ++i) {
        if (1. / (double(i) + 1) < prob) {
            return i;
        }
    }

    return 0;
}
