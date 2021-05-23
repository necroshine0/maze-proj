#include <iostream>
#include <vector>
#include <memory>
#include "bijection.h"
#include "randomgenerator.h"
#include "tensor.h"
#include "vertex.h"
#include "help_functions.h"
#include "discrete.h"
#include "maze.h"
#include "levenshtein.h"

using graph_int = std::vector<std::vector<int>>;
using namespace std;

int main() {
    setlocale(LC_ALL, "ru");
    string mazename = "nikolskaya.txt";
    //cout << "Введите файла с лабиринтом: ";
    //getline(cin, mazename);

    Maze mz = Construct_Maze("graphs/" + mazename);
    std::cout << "Лабиринт построен\n";

    // alert_vertexes(mz.GetBjn(), mz.GetGraphList(), mz.GetVertexes());
    // mz.GetTensor().PrintPaths(mz.GetBjn());
    discrete_vector DV;
    vector<string> U = read_file("files/temp.txt", 5);
    string U_1 = "ОМЭЯГЯМОЫОИКИЛИМЯГЯЭМИЛКДСШЕЛИМИМИКДСШРШСДСШЕЛИОЫОИМЯЗУЩТ";
    pair<vector<double>, int> answer = learn(mz, 16, U, DV, U_1, 0.05, 20);

    return 0;
}

// Все последовательности не должны выходить за пределы лабиринта
// Поэтому уберем все ребра от выходов к входам - для Никольской ТЫ и РЫ
// В этом случае можно оптимизировать класс SymTensor
