#pragma once
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <memory>
#include "vertex.h"
#include "bijection.h"
#include "randomgenerator.h"
#include "tensor.h"
#include "help_functions.h"

using graph_int = std::vector<std::vector<int>>;
extern RandomGenerator gen;

class Maze {
private:
    Symmetric_Tensor3D ST;
    Directions DIRS;
    graph_int gph;
    Bjn bjn;
    std::vector<Vertex> vertexes;

public:


    Maze(const Bjn& b,
            const std::vector<Vertex>& vs,
                const graph_int& tbl) {
        gph = tbl;
        bjn = b;
        vertexes = vs;

        graph_int matrix = construct_table(tbl);
        ST = floyd_warshall(matrix);

        DIRS = Directions(vertexes, matrix);
    }

    const Bjn GetBjn() const { return bjn; }
    const std::vector<Vertex> GetVertexes() const { return vertexes; }
    const Directions GetDirections() const { return DIRS; }
    const graph_int GetGraphList() const { return gph; }
    const Symmetric_Tensor3D GetTensor() const { return ST; }

    void MazeInfo() const {
        alert_vertexes(bjn, gph, vertexes);
        std::cout << '\n';

        std::cout << "ADJ LIST:\n";
        alert(gph);
        std::cout << '\n';

        std::cout << "DIRECTIONS:\n";
        DIRS.PrintDirections(bjn);
        std::cout << '\n';

        std::cout << "LEAST DISTS:\n";
        ST.PrintPaths(bjn);
        std::cout << "\n";
    }

    std::vector<int> gen_trivial_path(char start_c) const {
        int current_v = bjn[start_c];
        try {
            if (!vertexes[current_v].is_entry) {
                throw std::invalid_argument("Õ≈¬¿À»ƒÕ€… ¿–√”Ã≈Õ“");
            }
        }
        catch (std::invalid_argument& e) {
            std::cerr << e.what() << ": ";
            std::cerr << "—“¿–“Œ¬¿ﬂ ¬≈–ÿ»Õ¿ ƒŒÀ∆Õ€ ﬂ¬Àﬂ“‹—ﬂ “Œ◊ Œ… ¬’Œƒ¿ ¬ À¿¡»–»Õ“\n";
            exit(1);
        }

        std::vector<int> path;
        path.reserve(bjn.size());

        path.push_back(current_v);
        while (!vertexes[current_v].is_feed) {
            int u = random_choice(gph[current_v]);
            current_v = gph[current_v][u];
            path.push_back(current_v);
        }

        while (!vertexes[current_v].is_exit) {
            int u = random_choice(gph[current_v]);
            current_v = gph[current_v][u];
            path.push_back(current_v);
        }

        return path;
    }
};

// ‘‡ÈÎ ÒÓı‡ÌˇÚ¸ ‚ ANSI
Maze Construct_Maze(std::string filename) {
    setlocale(LC_ALL, "rus");
    std::ifstream file;
    file.open(filename);

    try {
        if (!file.is_open())
            throw std::runtime_error("Õ≈ ”ƒ¿ÀŒ—‹ Œ“ –€“‹ ‘¿…À");
    } catch (std::runtime_error & e) {
        std::cerr << e.what() << '\n';
        exit(1);
    }
    
    std::vector<char> vs;
    std::string line;

    getline(file, line);
    vs = split_char(line, ' ');
    Bjn bjn(vs);
    size_t n = bjn.size();
    std::vector<Vertex> vertexes(n);
    graph_int gph(n);

    for (size_t i = 0; i < n; ++i) {
          getline(file, line);
          char V;
          bool bv = 0, en = 0, ex = 0, feed = 0;
          int x = 0, y = 0;

          format(line, V, bv, en, ex, feed, x, y);
          vertexes[bjn[V]] = Vertex(bjn[V], bv, en, ex, feed, x, y);

          getline(file, line);
          vs = split_char(line, ' ');
          for (size_t j = 0; j != vs.size(); ++j) {
              gph[bjn[V]].push_back(bjn[vs[j]]);
          }

    }

    file.close();
    return Maze(bjn, vertexes, gph);
}
