#pragma once
#include <cstdint>
#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <cstring>
#include <map>
#include "maze.h"
#include "bijection.h"
#include "discrete.h"
#include "help_functions.h"

#define all(v) (v).begin(), (v).end()
double acc = 0.0001;
extern const int INF;
extern const int treshold;

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

int LevenshteinDistance(const std::string& s1, const std::string& s2) {
    size_t m(s1.size()), n(s2.size());

    if (m == 0) { return n; }
    if (n == 0) { return m; }

    // allocation below is not ISO-compliant,
    // it won't work with -pedantic-errors.

    std::vector<int> costs(n + 1);
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

int MinimalLevenshteinDistance(std::vector<std::string> realmice,
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

// Считаем, что каждая вероятность ограничена u_bound
std::vector<std::vector<double>> gen_grid(std::vector<double> l_bound, 
            std::vector<double> u_bound, double step) {
    std::vector<std::vector<double>> grid;
    grid.reserve(1000);
    std::vector<double> probs(4);

    probs[0] = l_bound[0];
    while (probs[0] < u_bound[0] + acc) {
        probs[1] = l_bound[1];
        while (probs[1] < u_bound[1] + acc) {
            probs[2] = l_bound[2];
            while (probs[2] < u_bound[2] + acc) {
                probs[3] = l_bound[3];
                while (probs[3] < std::min(u_bound[3], 1. - probs[0] - probs[1] - probs[2]) + acc ) {
                    if (probs[3] + probs[2] + probs[1] + probs[0] < 1 + acc &&
                        probs[3] + probs[2] + probs[1] + probs[0] > 1 - acc) {
                        grid.push_back(probs);
                    }

                    probs[3] += step;
                }

                probs[2] += step;
            }

            probs[1] += step;
        }

        probs[0] += step;
    }

    return grid;
}

/* bounds:
p0 => p0 +- step
p1 => p1 +- step
p2 => p2 +- step
p3 => p3 +- step
*/

std::vector<std::vector<double>> gen_grid(std::vector<double> _probs, 
            double step, double u_bound, int coef) {

    double _step = step / coef;
    u_bound = std::min(u_bound,
        _probs[0] + _probs[1] + _probs[2] + _probs[3] + 4 * step);
    std::vector<std::vector<double>> grid;
    grid.reserve(7000);
    for (size_t i = 0; i != 4; ++i) {
        _probs[i] = std::max(0., _probs[i] - step);
    }

    std::vector<double> probs(4);
    probs[0] = _probs[0];
    while (probs[0] <= std::min(u_bound, _probs[0] + 2 * step) + acc) {
        probs[1] = _probs[1];
        while (probs[1] <= std::min(u_bound, _probs[1] + 2 * step) + acc) {
            probs[2] = _probs[2];
            while (probs[2] <= std::min(u_bound, _probs[2] + 2 * step) + acc) {
                probs[3] = _probs[3];
                while (probs[3] <= std::min(_probs[3] + 2 * step,
                    std::min(u_bound, 1. - probs[0] - probs[1] - probs[2])) + acc) {
                    if (probs[3] + probs[2] + probs[1] + probs[0] < 1 + acc &&
                        probs[3] + probs[2] + probs[1] + probs[0] > 1 - acc) {
                        grid.push_back(probs);
                    }

                    probs[3] += _step;
                }

                probs[2] += _step;
            }

            probs[1] += _step;
        }

        probs[0] += _step;
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
                          discrete_vector& DV,
                          const std::string& U_1,
                          double step,
                          size_t eps) {

    std::vector<std::vector<double>> grid = gen_grid(
        { 0.05, 0.05, 0.05, 0.05 }, {0.9, 0.9, 0.9, 0.9}, step);
    std::cout << "Размер сетки: " << grid.size() << "\n";
    std::map<int, int> learnt;
    int last_res = 0;
    auto U_1_int = char_to_int(mz.GetBjn(), std::vector<char>(all(U_1)) );

    for (size_t k = 0; k != grid.size(); ++k) {
        DV.SetProbs(grid[k]);
        // G - множество дискретных последовательностей размера (trials + 1)
        std::vector<std::string> G;
        G.reserve(trials + 1);
        size_t N = gen.int_udist(2, 6);
        G = gen_paths_file_str(mz, DV, trials, U_1_int, N);
        for (size_t i = 1; i < trials + 1; ++i) {
            int res = INF;
            if (G[i].size() > 0 && G[i].size() < treshold) {
                res = MinimalLevenshteinDistance(U, G[i]);
            }
            // std::cout << "res: " << res << ' ' << G[i].size() << "\n";
            /*if (res <= eps) {
                std::cout << "COOL!\n";
                learnt[res] = k;
            }*/
            learnt[res] = k;
        }

        G.clear();
    }

    auto it = learnt.begin();
    if (!learnt.empty()) {
        std::cout << "Полученны оптимальные значения вероятностей на шаге " << it->second 
            << " со значением метрики " << it->first << '\n';
        std::cout << "Вероятности: ";
        alert(grid[it->second]);
        std::cout << "Выведем все результаты:\n";
        for (; it != learnt.end(); ++it) {
            std::cout << "Результат: " << it->first << ", шаг: " << it->second << ", вероятности: ";
            alert(grid[it->second]);
        }

        it = learnt.begin();
        return std::make_pair(grid[it->second], it->second);
    } else {
        std::cout << "Не удалось получить оптимальные значения вероятностей\n";
        return std::make_pair(std::vector<double>({ 0., 0., 0., 0. }), -1);
    }

    //// Второй этап обучения: уточнение
    //learnt.clear();
    //auto _probs = grid[it->second];
    //grid.clear();
    //int coef = 5;
    //grid = gen_grid(_probs, step, 0.9, coef);

    //for (size_t k = 0; k != grid.size(); ++k) {
    //    DV.SetProbs(grid[k]);
    //    std::vector<std::string> G = gen_paths_file_str(mz, DV, trials, U_1_int);
    //    for (size_t i = 1; i < trials + 1; ++i) {
    //        int res = MinimalLevenshteinDistance(U, G[i]);
    //        if (res <= eps) {
    //            learnt[res] = k;
    //        }
    //    }
    //}

    /*it = learnt.begin();
    if (!learnt.empty()) {
        std::cout << "Полученны оптимальные значения вероятностей на шаге " << it->second
            << " со значением метрики " << it->first << '\n';
        std::cout << "Вероятности: ";
        alert(grid[it->second]);
        return std::make_pair(grid[it->second], it->second);
    }
    else {
        std::cout << "Не удалось получить оптимальные значения вероятностей\n";
        return std::make_pair(std::vector<double>({ 0., 0., 0., 0. }), -1);
    }*/

    return std::make_pair(std::vector<double>({ 0., 0., 0., 0. }), -1);
}
