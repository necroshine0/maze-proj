#include <iostream>
#include <vector>
#include <memory>
#include "bijection.h"
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
    cout << "Nikolskaya Test\n";
    {
        Maze mz = Construct_Maze("graphs/nikolskaya.txt");
        Bjn bjn = mz.GetBjn();
        cout << "Size of bjn: " << sizeof(bjn) << '\n';
        auto ST = mz.GetTensor();
        cout << "Size of Tensor: " << sizeof(ST) << '\n';
        graph_int gph = mz.GetGraphList();
        cout << "Size of gph: " << sizeof(gph) << '\n';
        vector<Vertex> vertexes = mz.GetVertexes();
        cout << "Size of vertexes: " << sizeof(vertexes) << '\n';
        Directions DIRS = mz.GetDirections();
        cout << "Size of DIRS: " << sizeof(DIRS) << '\n';
        mz.MazeInfo();

        discrete_vector DV(0.22, 0.25, 0.25, 0.25);
        for (size_t i = 0; i < 5; ++i) {
            DV[i]->name();
        }
        cout << "\n";

        cout << "Symmetry test\n";
        {
            vector<char> vc_1S = { 'Ф', 'И', 'К', 'А' };
            vector<char> vc_2S = { 'Ы', 'О', 'Ф', 'Х', 'А', 'Д', 'С' };
            vector<char> vc_3S = { 'М', 'Я', 'Г', 'З' };
            vector<char> vc_4S = { 'Ш', 'Е', 'Л', 'И', 'М', 'Э', 'В' };

            vector<int> v_1S = char_to_int(bjn, vc_1S);
            vector<int> v_2S = char_to_int(bjn, vc_2S);
            vector<int> v_3S = char_to_int(bjn, vc_3S);
            vector<int> v_4S = char_to_int(bjn, vc_4S);

            DV[2]->method(mz, v_1S, v_1S.size());
            alert(int_to_char(bjn, v_1S));
            DV[2]->method(mz, v_2S, v_2S.size());
            alert(int_to_char(bjn, v_2S));
            DV[2]->method(mz, v_3S, v_3S.size());
            alert(int_to_char(bjn, v_3S));
            DV[2]->method(mz, v_4S, v_4S.size());
            alert(int_to_char(bjn, v_4S));
            cout << "\n";
        }

        cout << "Grip test\n";
        {
            vector<char> vc_1G = { 'А', 'Д', 'Х', 'Д', 'С', 'Ш', 'Л' };
            vector<char> vc_2G = { 'Щ', 'У', 'З', 'Я', 'Ц', 'Г', 'Ц', 'Ф', 'М', 'Э', 'Ж', 'В' };
            vector<char> vc_3G = { 'О', 'М', 'Я', 'З', 'Ц', 'Г', 'Я', 'Х', 'К', 'И', 'М', 'Э' };

            vector<int> v_1G = char_to_int(bjn, vc_1G);
            vector<int> v_2G = char_to_int(bjn, vc_2G);
            vector<int> v_3G = char_to_int(bjn, vc_3G);

            DV[3]->method(mz, v_1G, v_1G.size());
            alert(int_to_char(bjn, v_1G));
            DV[3]->method(mz, v_2G, v_2G.size());
            alert(int_to_char(bjn, v_2G));
            DV[3]->method(mz, v_3G, v_3G.size());
            alert(int_to_char(bjn, v_3G));
            cout << "\n";
        }

        cout << "Ring test\n";
        {
            vector<char> vc_1R = { 'Т' }; // FIXME: путь из входа в выход
            vector<char> vc_2R = { 'У' };
            vector<char> vc_3R = { 'Ы' };
            vector<int> v_1R = char_to_int(bjn, vc_1R);
            vector<int> v_2R = char_to_int(bjn, vc_2R);
            vector<int> v_3R = char_to_int(bjn, vc_3R);
            DV[4]->method(mz, v_1R, v_1R.size());
            alert(int_to_char(bjn, v_1R));
            DV[4]->method(mz, v_2R, v_2R.size());
            alert(int_to_char(bjn, v_2R));
            DV[4]->method(mz, v_3R, v_3R.size());
            alert(int_to_char(bjn, v_3R));
            cout << "\n";
        }
    }

    system("pause");
    system("cls");

    cout << "Chelnok Test\n";
    {
        Maze mz = Construct_Maze("graphs/chelnok.txt");
        Bjn bjn = mz.GetBjn();
        cout << "Size of bjn: " << sizeof(bjn) << '\n';
        auto ST = mz.GetTensor();
        cout << "Size of Tensor: " << sizeof(ST) << '\n';
        graph_int gph = mz.GetGraphList();
        cout << "Size of gph: " << sizeof(gph) << '\n';
        vector<Vertex> vertexes = mz.GetVertexes();
        cout << "Size of vertexes: " << sizeof(vertexes) << '\n';
        Directions DIRS = mz.GetDirections();
        cout << "Size of DIRS: " << sizeof(DIRS) << '\n';
        mz.MazeInfo();

        discrete_vector DV(0.22, 0.25, 0.25, 0.25);
        for (size_t i = 0; i < 5; ++i) {
            DV[i]->name();
        }
        cout << "\n";

        cout << "Symmetry test\n";
        {
            vector<char> vc_1S = { 'О', 'И', 'К', 'Х', 'Ф', 'Ц', 'Г' };
            vector<char> vc_2S = { 'Ы', 'Ю', 'Б', 'Е', 'Ш', 'Ё', 'Л' };
            vector<char> vc_3S = { 'М', 'Э', 'Й', 'Н', 'Щ', 'Ж', 'Т' };
            vector<char> vc_4S = { 'Г', 'Ц', 'Ф', 'Х', 'К', 'Л', 'Ю' };

            vector<int> v_1S = char_to_int(bjn, vc_1S);
            vector<int> v_2S = char_to_int(bjn, vc_2S);
            vector<int> v_3S = char_to_int(bjn, vc_3S);
            vector<int> v_4S = char_to_int(bjn, vc_4S);

            DV[2]->method(mz, v_1S, v_1S.size());
            alert(int_to_char(bjn, v_1S));
            DV[2]->method(mz, v_2S, v_2S.size());
            alert(int_to_char(bjn, v_2S));
            DV[2]->method(mz, v_3S, v_3S.size());
            alert(int_to_char(bjn, v_3S));
            DV[2]->method(mz, v_4S, v_4S.size());
            alert(int_to_char(bjn, v_4S));
            cout << "\n";
        }

        cout << "Grip test\n";
        {
            vector<char> vc_1G = { 'Д', 'Х', 'Ц', 'З', 'У', 'Н' };
            vector<char> vc_2G = { 'А', 'Х', 'Ф', 'М', 'Э', 'Й', 'Щ', 'Ж', 'Т', 'Ы', 'О', 'Ю' };
            vector<char> vc_3G = { 'П', 'С', 'Ё', 'Ш', 'Е', 'Л', 'И', 'К', 'Х', 'А' };

            vector<int> v_1G = char_to_int(bjn, vc_1G);
            vector<int> v_2G = char_to_int(bjn, vc_2G);
            vector<int> v_3G = char_to_int(bjn, vc_3G);

            DV[3]->method(mz, v_1G, v_1G.size());
            alert(int_to_char(bjn, v_1G));
            DV[3]->method(mz, v_2G, v_2G.size());
            alert(int_to_char(bjn, v_2G));
            DV[3]->method(mz, v_3G, v_3G.size());
            alert(int_to_char(bjn, v_3G));
            cout << "\n";
        }

        cout << "Ring test\n";
        {
            vector<char> vc_1R = { 'Р' };
            vector<char> vc_2R = { 'Ф' };
            vector<char> vc_3R = { 'А' };
            vector<char> vc_4R = { 'Ы' };
            vector<char> vc_5R = { 'Ю' };
            vector<int> v_1R = char_to_int(bjn, vc_1R);
            vector<int> v_2R = char_to_int(bjn, vc_2R);
            vector<int> v_3R = char_to_int(bjn, vc_3R);
            vector<int> v_4R = char_to_int(bjn, vc_4R);
            vector<int> v_5R = char_to_int(bjn, vc_5R);
            DV[4]->method(mz, v_1R, v_1R.size());
            alert(int_to_char(bjn, v_1R));
            DV[4]->method(mz, v_2R, v_2R.size());
            alert(int_to_char(bjn, v_2R));
            DV[4]->method(mz, v_3R, v_3R.size());
            alert(int_to_char(bjn, v_3R));
            DV[4]->method(mz, v_4R, v_4R.size());
            alert(int_to_char(bjn, v_4R));
            DV[4]->method(mz, v_5R, v_5R.size());
            alert(int_to_char(bjn, v_5R));
            cout << "\n";
        }
    }

    system("pause");
    system("cls");

    cout << "Berezhnoy Test\n";
    {
        Maze mz = Construct_Maze("graphs/berezhnoy.txt");
        Bjn bjn = mz.GetBjn();
        cout << "Size of bjn: " << sizeof(bjn) << '\n';
        auto ST = mz.GetTensor();
        cout << "Size of Tensor: " << sizeof(ST) << '\n';
        graph_int gph = mz.GetGraphList();
        cout << "Size of gph: " << sizeof(gph) << '\n';
        vector<Vertex> vertexes = mz.GetVertexes();
        cout << "Size of vertexes: " << sizeof(vertexes) << '\n';
        Directions DIRS = mz.GetDirections();
        cout << "Size of DIRS: " << sizeof(DIRS) << '\n';
        mz.MazeInfo();

        discrete_vector DV(0.22, 0.25, 0.25, 0.25);
        for (size_t i = 0; i < 5; ++i) {
            DV[i]->name();
        }
        cout << "\n";

        cout << "Symmetry test\n";
        {
            vector<char> vc_1S = { 'K', 'J', 'F', 'B', 'C', 'B', 'A' };
            vector<char> vc_2S = { 'P', 'O', 'K', 'G', 'C', 'G', 'F' };
            vector<char> vc_3S = { 'N', 'J', 'F', 'G', 'K', 'J', 'I' };
            vector<char> vc_4S = { 'D', 'C', 'B', 'C', 'G', 'H', 'L' };

            vector<int> v_1S = char_to_int(bjn, vc_1S);
            vector<int> v_2S = char_to_int(bjn, vc_2S);
            vector<int> v_3S = char_to_int(bjn, vc_3S);
            vector<int> v_4S = char_to_int(bjn, vc_4S);

            DV[2]->method(mz, v_1S, v_1S.size());
            alert(int_to_char(bjn, v_1S));
            DV[2]->method(mz, v_2S, v_2S.size());
            alert(int_to_char(bjn, v_2S));
            DV[2]->method(mz, v_3S, v_3S.size());
            alert(int_to_char(bjn, v_3S));
            DV[2]->method(mz, v_4S, v_4S.size());
            alert(int_to_char(bjn, v_4S));
            cout << "\n";
        }

        cout << "Grip test\n";
        {
            vector<char> vc_1G = { 'H', 'L', 'P', 'O', 'K', 'J' };
            vector<char> vc_2G = { 'D', 'C', 'B', 'A', 'B', 'F', 'G', 'K', 'L', 'P', 'O', 'K' };
            vector<char> vc_3G = { 'F', 'E', 'I', 'M', 'N', 'O', 'K', 'G', 'H', 'D' };

            vector<int> v_1G = char_to_int(bjn, vc_1G);
            vector<int> v_2G = char_to_int(bjn, vc_2G);
            vector<int> v_3G = char_to_int(bjn, vc_3G);

            DV[3]->method(mz, v_1G, v_1G.size());
            alert(int_to_char(bjn, v_1G));
            DV[3]->method(mz, v_2G, v_2G.size());
            alert(int_to_char(bjn, v_2G));
            DV[3]->method(mz, v_3G, v_3G.size());
            alert(int_to_char(bjn, v_3G));
            cout << "\n";
        }

        cout << "Ring test\n";
        {
            vector<char> vc_1R = { 'C' };
            vector<char> vc_2R = { 'F' };
            vector<char> vc_3R = { 'P' };
            vector<char> vc_4R = { 'D' };
            vector<char> vc_5R = { 'B' };
            vector<int> v_1R = char_to_int(bjn, vc_1R);
            vector<int> v_2R = char_to_int(bjn, vc_2R);
            vector<int> v_3R = char_to_int(bjn, vc_3R);
            vector<int> v_4R = char_to_int(bjn, vc_4R);
            vector<int> v_5R = char_to_int(bjn, vc_5R);
            DV[4]->method(mz, v_1R, v_1R.size());
            alert(int_to_char(bjn, v_1R));
            DV[4]->method(mz, v_2R, v_2R.size());
            alert(int_to_char(bjn, v_2R));
            DV[4]->method(mz, v_3R, v_3R.size());
            alert(int_to_char(bjn, v_3R));
            DV[4]->method(mz, v_4R, v_4R.size());
            alert(int_to_char(bjn, v_4R));
            DV[4]->method(mz, v_5R, v_5R.size());
            alert(int_to_char(bjn, v_5R));
            cout << "\n";
        }
    }

    return 0;
}