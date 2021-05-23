#pragma once
#include <random>
#include <ctime>

// Класс можно дополнить различными распределениями из
// https://en.cppreference.com/w/cpp/numeric/random

class RandomGenerator {
private:
    std::mt19937 gen;

public:
    RandomGenerator() {
        gen.seed(time(0));
    }

    double d_udist(double a, double b) {
        std::uniform_real_distribution<double> range(a, b);
        return range(gen);
    }

    int int_udist(int a, int b) {
        std::uniform_int_distribution<int> range(a, b);
        return range(gen);
    }

    friend int random_choice(const std::vector<int>& v);
};

static RandomGenerator gen;

int random_choice(const std::vector<int>& v){
    return gen.int_udist(0, v.size() - 1);
}
