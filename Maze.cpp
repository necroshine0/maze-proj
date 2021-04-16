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

#define all(v) (v).begin(), (v).end()
#define forn(i, N) for (size_t i = 0; i != (N); ++i)
#define foran(i, a, N) for (size_t i = a; i != (N); ++i)
#define forin(elem, v) for (auto elem : v)

using graph_int = std::vector<std::vector<int>>;

using namespace std;

int main() {
    setlocale(LC_ALL, "Russian");    
    {
        /*   (0, 0)
                А -- Е -- Л -- Р -- Х
                |    |    |    |    |
                Б -- Ж -- М -- С -- Ц
                |    |    |    |    |
                В -- З -- Н -- Т -- Ч
                |    |    |    |    |
                Г -- И -- О -- У -- Ю
                |    |    |    |    |
                Д -- К -- П -- Ф -- Я
        */
        Maze mz = Construct_Maze("graphs/graph_major_test.txt");
        Bjn bjn = mz.GetBjn();
        std::cout << "Size of bjn: " << sizeof(bjn) << '\n';
        auto ST = mz.GetTensor();
        std::cout << "Size of Tensor: " << sizeof(ST) << '\n';
        graph_int gph = mz.GetGraphList();
        std::cout << "Size of gph: " << sizeof(gph) << '\n';
        vector<Vertex> vertexes = mz.GetVertexes();
        std::cout << "Size of vertexes: " << sizeof(vertexes) << '\n';
        Directions DIRS = mz.GetDirections();
        std::cout << "Size of DIRS: " << sizeof(DIRS) << '\n';
        mz.MazeInfo();

        std::vector<char> vc_1 = {'А', 'Е', 'Ж', 'З', 'В', 'Г', 'И' };
        std::vector<char> vc_2 = { 'Е', 'А', 'Б', 'Ж', 'З', 'И', 'Г', 'Д' };
        std::vector<char> vc_3 = { 'А', 'Е', 'Ж', 'Б', 'Ж', 'З'};
        std::vector<char> vc_4 = { 'Х', 'Р', 'С', 'М', 'Н', 'Т', 'У', 'О', 'Н' };
        std::vector<char> vc_5 = { 'С', 'М', 'Н', 'Т', 'У', 'О', 'И' };
        std::vector<char> vc_6 = { 'М', 'С', 'Т', 'Ч', 'Ю' };

        std::vector<int> v_1 = char_to_int(bjn, vc_1);
        std::vector<int> v_2 = char_to_int(bjn, vc_2);
        std::vector<int> v_3 = char_to_int(bjn, vc_3);
        std::vector<int> v_4 = char_to_int(bjn, vc_4);
        std::vector<int> v_5 = char_to_int(bjn, vc_5);
        std::vector<int> v_6 = char_to_int(bjn, vc_6);

        discrete_vector DV(0.22, 0.25, 0.25, 0.25);
        for (size_t i = 0; i < 5; ++i) {
            DV[i]->name();
        }

        cout << "\n";

        DV[2]->method(mz, v_1, v_1.size());
        alert(int_to_char(bjn, v_1));
        DV[2]->method(mz, v_2, v_2.size());
        alert(int_to_char(bjn, v_2));
        DV[2]->method(mz, v_3, v_3.size());
        alert(int_to_char(bjn, v_3));
        DV[2]->method(mz, v_4, v_4.size());
        alert(int_to_char(bjn, v_4));
        DV[2]->method(mz, v_5, v_5.size());
        alert(int_to_char(bjn, v_5));
        DV[2]->method(mz, v_6, v_6.size());
        alert(int_to_char(bjn, v_6));

        cout << "\n";

        std::vector<char> vc_1G = { 'А', 'Е', 'Ж', 'З', 'В', 'Г', 'И' };
        std::vector<char> vc_2G = { 'Е', 'Ж', 'З', 'И', 'К', 'П', 'Ф', 'У', 'Ю', 'Я', 'Ю', 'Ч', 'Т', 'С', 'Ц', 'Ч', 'Т', 'Н' };

        std::vector<int> v_1G = char_to_int(bjn, vc_1G);
        std::vector<int> v_2G = char_to_int(bjn, vc_2G);

        DV[3]->method(mz, v_1G, v_1G.size());
        alert(int_to_char(bjn, v_1G));
        DV[3]->method(mz, v_2G, v_2G.size());
        alert(int_to_char(bjn, v_2G));

        cout << "\n";

        std::vector<char> vc_1I = { 'А', 'Е', 'Ж', 'З', 'В', 'Г', 'И' };
        std::vector<int> v_1I = char_to_int(bjn, vc_1I);
        DV[1]->method(mz, v_1I, v_1I.size());
        alert(int_to_char(bjn, v_1I));

        cout << "\n";

        std::vector<char> vc_1R = { 'А', 'Е', 'Ж', 'З', 'В', 'Г', 'И' };
        std::vector<int> v_1R = char_to_int(bjn, vc_1R);
        DV[4]->method(mz, v_1R, v_1R.size());
        alert(int_to_char(bjn, v_1R));
    }

    return 0;
}


// TODO:
// Добавить диагональные направления
// Сделать функцию заполнения таблицы на основе координат
// См. лабиринт Никольской: расстояние по диагонали будет равно |x1 - x2| + |y1 - y2|
// С рассчетом на это переделать оператор симметрии
