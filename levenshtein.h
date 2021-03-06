#pragma once
#include <cstdint>
#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <cstring>
#include <algorithm>
#include "maze.h"
#include "bijection.h"
#include "discrete.h"
#include "help_functions.h"

#define all(v) (v).begin(), (v).end()
double acc = 0.0001;
extern const int INF;
extern const int threshold;

// Вход: файл с последовательностями реальной мыши; число последних
// Выход: массив этих последовательностей
// Возможно, придется добавить парсинг, но пока что работаем с .txt,
// предполагая корректность данных
std::vector<std::wstring> read_file(const std::wstring& filename, int n) {
    std::wifstream  file;
    file.open(filename);

    try {
        if (!file.is_open())
            throw std::runtime_error("НЕ УДАЛОСЬ ОТКРЫТЬ ФАЙЛ");
    }
    catch (std::runtime_error& e) {
        std::cerr << e.what() << '\n';
        exit(1);
    }

    std::wstring line;
    getline(file, line);
    int q_lines = std::stoi(line.c_str());

    if (q_lines > n) {
        for (int i = 0; i != q_lines - n; ++i) {
            getline(file, line);
        }
    }

    std::vector<std::wstring> last_seqs(std::min(n, q_lines));
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

int LevenshteinDistance(const std::wstring& s1, const std::wstring& s2) {
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
    for (wchar_t const& c1 : s1) {
        costs[0] = i + 1;
        size_t corner = i, j = 0;
        for (wchar_t const& c2 : s2) {
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

int MinimalLevenshteinDistance(std::vector<std::wstring> realmice,
    const std::wstring& modelmouse) {

    size_t dist, min_dist = threshold;
    for (auto path : realmice) {
        dist = LevenshteinDistance(modelmouse, path);
        if (dist < min_dist) {
            min_dist = dist;
        }
    }

    return min_dist;
}

size_t MinimalLevenshteinDistance(std::wstring filename,
    const std::wstring& modelmouse, size_t n) {

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
                while (probs[3] < std::min(u_bound[3], 1. - probs[0] - probs[1] - probs[2]) + acc) {
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

struct OptimalData {
    std::vector<double> probs;
    int step;
    int res;

    OptimalData(std::vector<double> _pr = {}, int _st = -1, int _res = -1) :
        probs(_pr), step(_st), res(_res) {}

    bool operator<(const OptimalData& rhs) const {
        if (res != rhs.res)
            return res < rhs.res;
        if (step != rhs.step)
            return step < rhs.step;
        return false;
    }
};

// Вход: лабиринт; число генерируемых последовательностей; множество реальных последовательностей;
// массив всех мозможных веростностей; дискретный вектор (неконст);  первую строку (реальной мыши); eps
// Возврат: оптимальные вероятности и шаг, на котором они были получены в случае успеха
//          иначе: вектор нулей и -1
OptimalData learn(const Maze& mz, size_t trials,
    const std::vector<std::wstring> U, discrete_vector& DV, const std::wstring& U_1,
    double step, size_t eps) {

    // Открытие файла
    std::ofstream file("sequences.txt", std::ios_base::out | std::ios_base::app);
    try {
        if (!file.is_open())
            throw std::runtime_error("НЕ УДАЛОСЬ ОТКРЫТЬ ИЛИ СОЗДАТЬ ФАЙЛ");
    }
    catch (std::runtime_error& e) {
        std::cerr << e.what() << '\n';
        exit(1);
    }
    // Генерация сетки вероятностей
    std::vector<std::vector<double>> grid = gen_grid(
        { 0.05, 0.05, 0.05, 0.05 }, { 0.9, 0.9, 0.9, 0.9 }, step);
    std::cout << "Размер сетки: " << grid.size() << "\n\n";

    std::vector<OptimalData> learnt;
    auto U_1_int = char_to_int(mz.GetBjn(), std::vector<wchar_t>(all(U_1)));

    // G - множество дискретных последовательностей размера (trials + 1)
    for (size_t k = 0; k != grid.size(); ++k) {
        DV.SetProbs(grid[k]);
        int quant = 0;

        // Запишем в файл первую строку
        for (auto it = U_1.begin(); it != U_1.end(); ++it) { file << *it; }
        file << '\n';
        std::vector<int> seq = U_1_int;
        for (size_t i = 0; i < trials; ++i) {
            // Выбираем N - число применений ДО к одной последовательности
            int N = gen.int_udist(seq.size() / 30 + 1, seq.size() / 20 + 3);
            // Создаем новую последовательность из seq
            seq = gen_discrete_path(mz, DV, seq, N);
            // Если размер последовательности больше порога - никаких преобразований не делаем
            if (seq.size() >= threshold) { break; }
            // Конвертируем и выводим в файл
            auto seq_char = int_to_char_str(mz.GetBjn(), seq);
            for (auto it = seq_char.begin(); it != seq_char.end(); ++it) { file << *it; }
            file << '\n';
            // Увеличиваем число ДО-применений
            quant += N;

            // Получаем значение по метрике
            int res = INF;
            if (seq_char.size() > 0) {
                res = MinimalLevenshteinDistance(U, seq_char);
            }

            if (res <= eps) {
                learnt.push_back(OptimalData(DV.GetProbs(), quant, res));
            }
        }

        file << '\n';
    }

    file.close();
    // Отсортируем вектор сначала по значению результата, затем по значению шага по неубыванию
    std::sort(all(learnt));
    auto it = learnt.begin();
    if (!learnt.empty()) {
        std::cout << "   Полученны оптимальные значения вероятностей с числом применений ДО " << it->step
            << " со значением метрики " << it->res << '\n';
        std::cout << "   Вероятности: ";
        alert(it->probs);
        std::cout << "   Выведем все результаты:\n";
        for (; it != learnt.end(); ++it) {
            std::cout << "   Значение метрики: " << it->res << ", число преобразований: " << it->step << ", вероятности: ";
            alert(it->probs);
        }

        return *learnt.begin();
    }
    else {
        std::cout << "   Не удалось получить оптимальные значения вероятностей\n";
    }

    return OptimalData();
}



// Функция строит дискретные строки из текущей и сраниваем их с реальными
void compare(const Maze& mz, size_t trials, const std::vector<std::wstring> U,
    discrete_vector& DV, const std::wstring& U_1, const std::vector<double>& probs) {

    if (probs.size() == 0) { return; }

    auto U_1_int = char_to_int(mz.GetBjn(), std::vector<wchar_t>(all(U_1)));

    DV.SetProbs(probs);
    std::vector<int> seq = U_1_int;
    int quant = 0;
    for (size_t i = 0; i < trials; ++i) {
        std::cout << "   " << i + 1 << ". ";
        // Выбираем N - число применений ДО к одной последовательности
        int N = gen.int_udist(seq.size() / 30 + 1, seq.size() / 20 + 3);
        // Создаем новую последовательность из seq
        seq = gen_discrete_path(mz, DV, seq, N);
        // Если размер последовательности больше порога - никаких преобразований не делаем
        if (seq.size() > threshold) {
            std::cout << "   Получена последовательность, превышающая величину порога\n";
            break;
        }

        quant += N;
        // Конвертируем
        auto seq_char = int_to_char_str(mz.GetBjn(), seq);

        // Получаем значение по метрике
        int res = INF;
        if (seq_char.size() > 0) {
            res = MinimalLevenshteinDistance(U, seq_char);
        }

        std::cout << "\tЗначение метрики: " << res << ",\t число преобразований: " << N << ", \tнакопительное число преобразований: " << quant << "\n";
    }
}