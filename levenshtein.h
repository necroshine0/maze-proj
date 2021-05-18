#pragma once
#include <cstdint>
#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <cstring>
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
