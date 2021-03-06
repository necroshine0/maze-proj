#include <iostream>
#include <vector>
#include "bijection.h"
#include "randomgenerator.h"
#include "tensor.h"
#include "vertex.h"
#include "help_functions.h"
#include "discrete.h"
#include "maze.h"
#include "string"
#include "levenshtein.h"

using graph_int = std::vector<std::vector<int>>;
using namespace std;
std::vector<Maze> mzs;
discrete_vector DV;
extern const int threshold;

int main() {
    system("chcp 1251");
    system("cls");
    string script;
    int id;

    while (true) {
        string command = "";
        cout << "Ожидание команды...\n";
        getline(cin, script);
        auto list = split_str(script, ' ');
        command = list[0];

        if (command == "exit" || command == "ex") { return 0; }
        if (command == "build" || command == "bd") {
            string mazename;
            cout << "Введите имя файла с лабиринтом: ";
            getline(cin, mazename);
            mzs.push_back(Construct_Maze("graphs/" + mazename));
            cout << "Построен лабиринт с номером " << mzs.size() - 1 << "\n";
            continue;
        }
        if (command == "mazeinfo" || command == "info") {
            if (list.size() == 2) {
                id = stol(list[1]);
            }
            else { id = (int)mzs.size() - 1; }

            if (id != -1 && id < mzs.size()) {
                mzs[id].MazeInfo();
                continue;
            }
            else if (id != -1 && id >= mzs.size()) { cout << "Невозможно выполнить команду: нет лабиринта с таким номером\n"; }
            else { cout << "Невозможно выполнить команду: ни один лабиринт не создан\n"; }
        }
        if (command == "dirs") {
            if (list.size() == 2) {
                id = stol(list[1]);
            }
            else { id = (int)mzs.size() - 1; }

            if (id != -1 && id < mzs.size()) {
                mzs[id].GetDirections().PrintDirections(mzs[id].GetBjn());
                continue;
            }
            else if (id != -1 && id >= mzs.size()) { cout << "Невозможно выполнить команду: нет лабиринта с таким номером\n"; }
            else { cout << "Невозможно выполнить команду: ни один лабиринт не создан\n"; }
        }
        if (command == "verts") {
            if (list.size() == 2) {
                id = stol(list[1]);
            }
            else { id = (int)mzs.size() - 1; }

            if (id != -1 && id < mzs.size()) {
                alert_vertexes(mzs[id].GetBjn(), mzs[id].GetGraphList(), mzs[id].GetVertexes());
                continue;
            }
            else if (id != -1 && id >= mzs.size()) { cout << "Невозможно выполнить команду: нет лабиринта с таким номером\n"; }
            else { cout << "Невозможно выполнить команду: ни один лабиринт не создан\n"; }
        }
        if (command == "least") {
            if (list.size() == 2) {
                id = stol(list[1]);
            }
            else { id = (int)mzs.size() - 1; }

            if (id != -1 && id < mzs.size()) {
                mzs[id].GetTensor().PrintPaths(mzs[id].GetBjn());
                continue;
            }
            else if (id != -1 && id >= mzs.size()) { cout << "Невозможно выполнить команду: нет лабиринта с таким номером\n"; }
            else { cout << "Невозможно выполнить команду: ни один лабиринт не создан\n"; }
        }
        if (command == "alist") {
            if (list.size() == 2) {
                id = stol(list[1]);
            }
            else { id = (int)mzs.size() - 1; }

            if (id != -1 && id < mzs.size()) {
                alert(mzs[id].GetGraphList(), mzs[id].GetBjn());
                continue;
            }
            else if (id != -1 && id >= mzs.size()) { cout << "Невозможно выполнить команду: нет лабиринта с таким номером\n"; }
            else { cout << "Невозможно выполнить команду: ни один лабиринт не создан\n"; }
        }
        if (command == "gen") {
            if (list[1] == "first") {
                int num = atoi(list[2].c_str());
                wchar_t start_c = list[3].front();
                if (num != -1 && num < mzs.size()) {
                    vector<int> path = mzs[num].gen_trivial_path(start_c);
                    vector<wchar_t> path_str = int_to_char(mzs[num].GetBjn(), path);
                    if (list.size() == 5) {
                        std::wofstream file(list[4], std::ios_base::out | std::ios_base::app);
                        try {
                            if (!file.is_open())
                                throw std::runtime_error("НЕ УДАЛОСЬ ОТКРЫТЬ ИЛИ СОЗДАТЬ ФАЙЛ");
                        }
                        catch (std::runtime_error& e) {
                            std::cerr << e.what() << '\n';
                            exit(1);
                        }

                        for (auto it = path_str.begin(); it != path_str.end(); ++it) { file << *it; }
                        file << L'\n';
                        file.close();
                        cout << "Последовательность записана в файл\n";
                    }
                    else { cout << "Необходимо указать имя файла\n"; }
                }
                else if (num != -1 && num >= mzs.size()) { cout << "Невозможно выполнить команду: нет лабиринта с таким номером\n"; }
                else { cout << "Невозможно выполнить команду: ни один лабиринт не создан\n"; }
            }
            else if (list[1] == "discrete") {
                int num = stol(list[2]);
                int N = stol(list[3]);
                vector<double> probs;
                for (size_t i = 4; i != 8; ++i) {
                    string p = list[i];
                    p[1] = ',';
                    probs.push_back(stod(p));
                }

                discrete_vector DV(probs);
                std::wfstream file(list.back(), ios_base::in | ios_base::out | ios_base::app);
                try {
                    if (!file.is_open())
                        throw std::runtime_error("НЕ УДАЛОСЬ ОТКРЫТЬ ИЛИ СОЗДАТЬ ФАЙЛ");
                }
                catch (std::runtime_error& e) {
                    std::cerr << e.what() << '\n';
                    exit(1);
                }


                file.seekg(-1, ios_base::end);
                wchar_t ch;
                file.get(ch);
                while (ch == L'\n') { file.seekg(-4, ios_base::cur); file.get(ch); }

                while (true) {
                    file.get(ch);

                    if ((int)file.tellg() <= 1) {
                        file.seekg(0);
                        break;
                    }
                    else if (ch == L'\n') { break; }
                    else { file.seekg(-2, ios_base::cur); }
                }

                wstring last;
                getline(file, last);
                if (last.size() == 0) {
                    cout << "Не найдена первая последовательность\n";
                    cout << "Генерация не удалась\n";
                    continue;
                }

                auto seq = char_to_int(mzs[num].GetBjn(), std::vector<wchar_t>(all(last)));
                for (size_t i = 0; i < N; ++i) {
                    int Q = gen.int_udist(seq.size() / 30 + 1, seq.size() / 20 + 3);
                    seq = gen_discrete_path(mzs[num], DV, seq, Q);
                    if (seq.size() >= threshold) { break; }
                    auto seq_char = int_to_char_str(mzs[num].GetBjn(), seq);
                    for (auto it = seq_char.begin(); it != seq_char.end(); ++it) { file << *it; }
                    file << L'\n';
                }

                file << L'\n';
                file.close();
                cout << "Последовательности сгенерированы\n";
            }
            else { cout << "Такой команды не существует\n"; }
            continue;
        }

        cout << "Такой команды не существует\n";
    }

    //string mazename = "nikolskaya.txt";
    ////cout << "Введите файла с лабиринтом: ";
    ////getline(cin, mazename);

    //Maze mz = Construct_Maze("graphs/" + mazename);
    //std::cout << "Лабиринт построен\n";
    //discrete_vector DV;
    //vector<string> U = read_file("files/temp.txt", 5);
    //string U_1 = "ОМЭЯГЯМОЫОИКИЛИМЯГЯЭМИЛКДСШЕЛИМИМИКДСШРШСДСШЕЛИОЫОИМЯЗУЩТ";
    //OptimalData answer = learn(mz, 15, U, DV, U_1, 0.05, 6);
    //cout << "\n";
    //compare(mz, 15, U, DV, U_1, answer.probs);
    return 0;
}