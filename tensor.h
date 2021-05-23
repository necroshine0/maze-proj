#pragma once
#include <iostream>
#include <vector>
#include "vertex.h"
#include "help_functions.h"

#define all(v) (v).begin(), (v).end()
#define forn(i, N) for (size_t i = 0; i != (N); ++i)
#define foran(i, a, N) for (int i = a; i != (N); ++i)
#define forin(elem, v) for (auto elem : v)

using graph_int = std::vector<std::vector<int>>;
using tensor_3D = std::vector<std::vector<std::vector<int>>>;
extern const int INF;

class Tensor3D {
private:
    std::vector<std::vector<std::vector<int>>> ST;
    std::vector<Vertex> vertexes;
    size_t size = 0;

public:
    Tensor3D() {}
    Tensor3D(size_t n) { 
        ST.resize(n); 
        for (size_t i = 0; i != n; ++i) { ST[i].resize(n); }
        size = n;

    }

    void SetVertexes(const std::vector<Vertex>& vs) { vertexes = vs; }

    const std::vector<std::vector<int>> operator[] (size_t i) const { return ST[i]; }
    std::vector<std::vector<int>>& operator[] (size_t i) { return ST[i]; }

    const void PrintPaths(const Bjn& bjn) const {
        for (size_t u = 0; u != size; ++u) {
            for (size_t v = 0; v != size; ++v) {
                if (u == v) { continue; }
                std::cout << bjn[u] << "-" << bjn[v] << ":";
                for (auto it = ST[u][v].begin(); it != ST[u][v].end(); ++it) {
                    std::cout << ' ' << bjn[*it];
                }

                std::cout << '\n';
            }
        }
    }

    // Оператор двойной индексации
    std::vector<int> operator() (size_t i, size_t j) const {
        if (i == j) { return std::vector<int>(); }
        std::vector<int> path(ST[i][j].size() - 1);
        foran(t, 1, ST[i][j].size()) {
            path[t - 1] = ST[i][j][t];
        }

        return path;
    }
};

// Задается матрица смежности по списку с весами 1
graph_int construct_table(const graph_int& gph, const std::vector<Vertex>& vs) {
    size_t n = gph.size();
    graph_int tbl(n, std::vector<int>(n));

    forn(i, n) {
        tbl[i].assign(n, INF);
        tbl[i][i] = 0;
        for ( auto j : gph[i]) {
            if (i != j) {
                tbl[i][j] = std::abs(vs[i].GetX() - vs[j].GetX()) +
                            std::abs(vs[i].GetY() - vs[j].GetY());
            }
        }
    }

    return tbl;
}

// Благодарность: Владимиру Фолунину, преподавателю алгоритмов НИУ ВШЭ
Tensor3D floyd_warshall(const graph_int& gph, const std::vector<Vertex> VS) {
    size_t n = gph.size();
    graph_int result = gph;
    graph_int H(n, std::vector<int>(n));

    forn(i, n) {
        forn(j, n) {
            if (gph[i][j] != INF && i != j) {
                H[i][j] = j;
            }
        }
    }

    forn(middle, n) {
        forn(from, n) {
            forn(to, n) {
                if (result[from][middle] < INF && result[middle][to] < INF &&
                    result[from][middle] + result[middle][to] < result[from][to]) {

                    result[from][to] = result[from][middle] + result[middle][to];
                    if (middle != to) {
                        H[from][to] = H[from][middle];
                    }
                }
            }
        }
    }

    Tensor3D recovered(n);
    recovered.SetVertexes(VS);

    std::vector<int> path;
    forn(i, n) {
        forn(j, n) {
            if (i != j) {
                size_t k = i;
                while (k != j) {
                    path.push_back(k);
                    k = H[k][j];
                }
                path.push_back(j);
                recovered[i][j] = path;
                path.clear();
            } else {
                recovered[i][j] = std::vector<int>({});
            }
        }
    }

    return recovered;
}
