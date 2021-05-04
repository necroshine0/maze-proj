#pragma once
#include <iostream>
#include <vector>
#include <string>
#include <cctype>
#include <memory>
#include "bijection.h"
#include "vertex.h"

using graph_int = std::vector<std::vector<int>>;
extern const int INF;

// Вывод вектора
template <typename T>
void alert(const std::vector<T>& v) {
    for (size_t i = 0; i != v.size(); ++i) {
        std::cout << v[i] << ' ';
    }

    std::cout << '\n';
}

// Вывод таблицы
template <typename T>
void alert(const std::vector<std::vector<T>>& tbl) {
    for (size_t i = 0; i < tbl.size(); ++i) {
        std::cout << i << ":\t";
        for (size_t j = 0; j < tbl[i].size(); ++j) {
            std::cout << tbl[i][j] << "\t";
        }
        std::cout << '\n';
    }
}

// Вывод таблицы в символьном формате
template <typename T>
void alert(const std::vector<std::vector<T>>& tbl, const Bjn& bjn) {
    for (size_t i = 0; i < tbl.size(); ++i) {
        std::cout << bjn[i] << ":  ";
        for (size_t j = 0; j < tbl[i].size(); ++j) {
            std::cout << bjn[tbl[i][j]] << "  ";
        }
        std::cout << '\n';
    }
}

// Вывод матрицы смежности
template <typename T>
void alert_adj_table(const std::vector<std::vector<T>>& tbl, const Bjn& bjn) {
    std::cout << " \t";
    for (size_t i = 0; i < tbl.size(); ++i) {
        std::cout << bjn[i] << " \t";
    }
    std::cout << "\n";
    for (size_t i = 0; i < tbl.size(); ++i) {
        std::cout << bjn[i] << ": \t";
        for (size_t j = 0; j < tbl[i].size(); ++j) {
            if (tbl[i][j] == INF) {
                std::cout << "INF \t";
            }
            else if (tbl[i][j] == 0) {
                std::cout << " \t";
            }
            else {
                std::cout << tbl[i][j] << " \t";
            }
        }

        std::cout << '\n';
    }

    std::cout << '\n';
}

// Как в питоне
std::vector<std::string> split_str(const std::string& str, char delimiter) {
    std::vector<std::string> v;
    std::string subline;

    for (size_t i = 0; i < str.size(); ++i) {
        if (str[i] != delimiter) { subline += str[i]; }
        else if (!subline.empty()) {
            v.push_back(subline);
            subline = "";
        }
    }

    if (*(--str.end()) != delimiter)
        v.push_back(subline);
    return v;
}

std::vector<char> split_char(const std::string& str, char delimiter) {
    std::vector<char> chars;
    for (size_t i = 0; i < str.size(); ++i) {
        if (str[i] != delimiter) chars.push_back(str[i]);
    }

    return chars;
}

void PrintAdjList(const graph_int& gph, const Bjn& bjn) {
    for (size_t i = 0; i < gph.size(); ++i) {
        std::cout << bjn[i] << ": ";
        for (size_t j = 0; j < gph[i].size(); ++j) {
            std::cout << bjn[gph[i][j]] << '\t';
        }

        std::cout << '\n';
    }
}

void format(std::string& line,
            char& V, bool& bv, bool& en, bool& ex, bool& feed,
            int& x, int& y) {

    V = line[0];
    for (size_t i = 0; i < line.size(); ++i) {
        if (!isdigit(line[i]) && line[i] != '-') line[i] = ' ';
    }

    auto v = split_str(line, ' ');

    bv = atoi(v[0].c_str());
    if (bv) {
        en = atoi(v[1].c_str());
        ex = atoi(v[2].c_str());
        feed = atoi(v[3].c_str());
        x = atoi(v[4].c_str());
        y = atoi(v[5].c_str());
    }
    else {
        en = false;
        ex = false;
        feed = false;
        x = atoi(v[1].c_str());
        y = atoi(v[2].c_str());
    }
    
}
