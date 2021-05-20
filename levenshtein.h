#pragma once
#include <cstdint>
#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <cstring>
#include "maze.h"
#include "bijection.h"
#include "discrete.h"
#include "help_functions.h"

#define all(v) (v).begin(), (v).end()

// Вход: файл с последовательностями реальной мыши; число последних
// Выход: массив этих последовательностей
// Возможно, придется добавить парсинг, но пока что работаем с .txt,
// предполагая корректность данных
std::vector<std::string> read_file(const std::string& filename, size_t n) {
    std::ifstream file;
    file.open(filename);

    try {
        if (!file.is_open())
            throw std::runtime_error("НЕ УДАЛОСЬ ОТКРЫТЬ ФАЙЛ");
    }
    catch (std::runtime_error& e) {
        std::cerr << e.what() << '\n';
        exit(1);
    }

    std::string line;
    getline(file, line);
    size_t q_lines = std::atoi(line.c_str());

    if (q_lines > n) {
        for (size_t i = 0; i != q_lines - n; ++i) {
            getline(file, line);
        }
    }

    std::vector<std::string> last_seqs(std::min(n, q_lines));
    for (size_t i = 0; i != std::min(n, q_lines); ++i) {
        getline(file, line);
        // Если нужно, то сюда можно вписать формат
        last_seqs[i] = line;
    }

    file.close();
    return last_seqs;
}

// https://rosettacode.org/wiki/Levenshtein_distance#C.2B.2B
// Compute Levenshtein Distance
// Martin Ettl, 2012-10-05
// T = { string, VString }

size_t LevenshteinDistance(const std::string& s1, const std::string& s2) {
    size_t m(s1.size()), n(s2.size());

    if (m == 0) { return n; }
    if (n == 0) { return m; }

    // allocation below is not ISO-compliant,
    // it won't work with -pedantic-errors.

    std::vector<size_t> costs(n + 1);
    for (size_t k = 0; k <= n; k++) {
        costs[k] = k;
    }

    size_t i = 0;
    for (char const& c1 : s1) {
        costs[0] = i + 1;
        size_t corner = i, j = 0;
        for (char const& c2 : s2) {
            size_t upper = costs[j + 1];
            if (c1 == c2) costs[j + 1] = corner;
            else {
                size_t t(upper < corner ? upper : corner);
                costs[j + 1] = (costs[j] < t ? costs[j] : t) + 1;
            }

            corner = upper;
            ++j;
        }

        ++i;
    }

    return costs[n];
}

size_t MinimalLevenshteinDistance(std::vector<std::string> realmice,
    const std::string& modelmouse) {

    size_t dist, min_dist = 500;
    for (auto path : realmice) {
        dist = LevenshteinDistance(modelmouse, path);
        if (dist < min_dist) {
            min_dist = dist;
        }
    }

    return min_dist;
}

size_t MinimalLevenshteinDistance(std::string filename,
            const std::string& modelmouse, size_t n) {

    auto last_seqs = read_file(filename, n);
    return MinimalLevenshteinDistance(last_seqs, modelmouse);
}

std::vector<std::vector<double>> gen_grid(double step, double u_bound) {
    std::vector<std::vector<double>> grid;
    grid.reserve(1000);
    std::vector<double> probs(4);

    probs[0] = 0.;
    while (probs[0] <= u_bound) {
        probs[1] = 0.;
        while (probs[1] <= u_bound) {
            probs[2] = 0.;
            while (probs[2] <= u_bound) {
                probs[3] = 0.;
                // Это условие сокращает число итераций цикла с 10000 до 632 при u_bound = 0.9
                while (probs[3] <= (0.9 - probs[0] - probs[1] - probs[2] ) ) {
                    probs[3] += step;
                    grid.push_back(probs);
                }

                probs[2] += step;
            }

            probs[1] += step;
        }

        probs[0] += step;
    }

    return grid;
}

// Вход: лабиринт; число генерируемых последовательностей; множество реальных последовательностей;
// массив всех мозможных веростностей; дискретный вектор (неконст);  первую строку (реальной мыши); eps
// Возврат: оптимальные вероятности и шаг, на котором они были получены в случае успеха
//          иначе: вектор нулей и -1
std::pair<std::vector<double>, int> learn(const Maze& mz,
                          size_t trials,
                          const std::vector<std::string> U,
                          const std::vector<std::vector<double>> grid,
                          discrete_vector& DV,
                          const std::string& U_1,
                          size_t eps) {

    int last_step = -1;
    auto U_1_int = char_to_int(mz.GetBjn(), std::vector<char>(all(U_1)) );
    for (size_t k = 0; k != grid.size(); ++k) {

        DV.SetProbs(grid[k]);
        // G - множество дискретных последовательностей размера (trials + 1)
        auto G = gen_paths_file_str(mz, DV, trials, U_1_int);

        for (size_t i = 1; i < trials; ++i) {
            size_t res = MinimalLevenshteinDistance(U, G[i]);
            if (res < eps) {
                last_step = k;
                break;
            }
        }
    }

    if (last_step != -1) {
        std::cout << "Полученны оптимальные значения вероятностей на шаге " << last_step << '\n';
        std::cout << "Их значения: ";
        alert(grid[last_step]);
        return std::make_pair(grid[last_step], last_step);
    } else {
        std::cout << "Не удалось получить оптимальные значения вероятностей\n";
    }

    return std::make_pair(std::vector<double>({ 0., 0., 0., 0. }), -1);
}
