#pragma once
#include <iostream>
#include <fstream>
#include <vector>
#include <memory>
#include <algorithm>
#include <string>
#include "help_functions.h"
#include "tensor.h"
#include "maze.h"

extern RandomGenerator gen;
extern const int INF;
const int treshold = 500;
extern double acc;

std::vector<int> subseq(const std::vector<int>& v, size_t a, size_t b) {
    std::vector<int> subseq;
    subseq.reserve(6);
    for (size_t i = a; i < b; ++i) {
        subseq.push_back(v[i]);
    }

    return subseq;
}

class DiscreteOperator {
protected:
    double probability = .0;

public:
    DiscreteOperator() {
        probability = .0;
    }

    DiscreteOperator(double prob) {
        probability = prob;
    }

    virtual const double GetProb() const { return probability; }
    virtual void SetProb(double prob) { probability = prob; }

    virtual void method(const Maze& mz, std::vector<int>& v, size_t subseq_size) const {}
    virtual void name() const {}
};

class Inverse : public DiscreteOperator {
public:
    Inverse(double prob) : DiscreteOperator(prob) {};

    void method(const Maze& mz, std::vector<int>& v, size_t subseq_size) const override {
        if (probability == .0) return;
        if (subseq_size > v.size()) { return; }

        size_t fixed_size = v.size();
        for (size_t i = 1; i < subseq_size; ++i) {
            v.push_back(v[fixed_size - i - 1]);
        }
    }

    void name() const override { std::cout << "I'm an Inverse!\n"; }
};

class Symmetry : public DiscreteOperator {
public:

    Symmetry(double prob) : DiscreteOperator(prob) {}

    void method(const Maze& mz, std::vector<int>& v, size_t subseq_size) const override {
        if (probability == .0) return;
        if (subseq_size > v.size()) { return; }
        
        auto vs = mz.GetVertexes();
        std::vector<int> subv = subseq(v, v.size() - subseq_size, v.size());
        Directions DIRS = mz.GetDirections();
        auto str_v = DIRS.convert_to_dirs(subv);
        int first_vert = subv.front();
;
        // Теоретически можно использовать и горизонтальный тип разреза
        std::string type = "vertical";

        auto gph = mz.GetGraphList();
        auto borders = mz.GetBorders(type);
        // Симметричная вершина - это вершина с координатами:
        int x = 0, y = 0;
        if (type == "vertical") {
            x = borders.second - (vs[first_vert].GetX() - borders.first);
            y = vs[first_vert].GetY();
        } else if (type == "horizontal") {
            x = vs[first_vert].GetX();
            y = borders.second - (vs[first_vert].GetY() - borders.first);
        }

        int new_vert = -1;
        for (auto v : vs) {
            if (v.GetX() == x && v.GetY() == y) {
                new_vert = v.number;
                break;
            }
        }

        try {
            if (new_vert == -1) {
                throw std::invalid_argument("ОШИБКА ПРЕОБРАЗОВАНИЯ");
            }
        } catch (std::invalid_argument& e) {
            std::cerr << e.what() << ": ";
            std::cerr << "НЕВОЗМОЖНО ПОСТРОИТЬ СИММЕТРИЧНЫЙ ПУТЬ ДЛЯ ПУТИ [ ";
            for (size_t i = 0; i != subv.size(); ++i) {
                std::cerr << mz.GetBjn()[subv[i]] << ' ';
            }

            std::cerr << "]\n";
            std::cerr << "ВОЗМОЖНО, ЛАБИРИНТ НЕ ЯВЛЯЕТСЯ СИММЕТРИЧНЫМ\n";
            exit(1);
        }

        std::vector<int> tmp_v = mz.GetTensor()(subv.back(), new_vert);
        if (tmp_v.size() > 0) {
            for (size_t i = 0; i < tmp_v.size() - 1; ++i) { v.push_back(tmp_v[i]); }
        }

        inverse_dirs(str_v, type);
        subv = DIRS.convert_to_v(str_v, new_vert);
        for (size_t i = 0; i < subv.size(); ++i) { v.push_back(subv[i]); }
    }

    void name() const override { std::cout << "I'm a Symmetry!\n"; }
};

class Grip : public DiscreteOperator {
public:

    Grip(double prob) : DiscreteOperator(prob) {};

    void method(const Maze& mz, std::vector<int>& v, size_t subseq_size) const override {
        if (probability == .0) return;
        if (v.size() < 8) { return; }
        if (subseq_size >= v.size()) { subseq_size = 2 * v.size() / 3; }

        size_t from = v[v.size() - subseq_size];
        size_t to = *(--v.end());
        for (size_t i = 0; i < subseq_size - 1; ++i) {
            v.pop_back();
        }

        std::vector<int> u = mz.GetTensor()(from, to);
        for (size_t i = 0; i < u.size(); ++i) {
            v.push_back(u[i]);
        }
    }

    void name() const override { std::cout << "I'm a Grip!\n"; }
};

class Ring : public DiscreteOperator {
public:

    Ring(double prob) : DiscreteOperator(prob) {};

    void method(const Maze& mz, std::vector<int>& v, size_t subseq_size) const override {
        if (probability == .0) return;

        std::vector<Vertex> VS = mz.GetVertexes();
        int fixed_v = *(--v.end());
        graph_int gph = mz.GetGraphList();
        int current_v = fixed_v;

        while (true) {
            size_t u = random_choice(gph[current_v]);
            current_v = gph[current_v][u];
            v.push_back(current_v);

            if (VS[current_v].bio_value) { break; }
        }

        auto path = mz.GetTensor()(current_v, fixed_v);

        for (size_t i = 0; i < path.size(); ++i) {
            v.push_back(path[i]);
        }
    }

    void name() const override { std::cout << "I'm a Ring!\n"; }
};

class Empty : public DiscreteOperator {
public:

    Empty(double prob) : DiscreteOperator(prob) {};
    void method(const Maze& mz, std::vector<int>& v, size_t subseq_size) const override {
        return;
    }

    void name() const override { std::cout << "I'm an Empty!\n"; }
};

class discrete_vector {
private:
    std::vector<std::unique_ptr<DiscreteOperator>> DOs;
    size_t indexes[5]; // ISGRE

public:
    discrete_vector() {
        DOs.reserve(5);
        DOs.push_back(std::unique_ptr<DiscreteOperator>(new Inverse(.0)));
        DOs.push_back(std::unique_ptr<DiscreteOperator>(new Symmetry(.0)));
        DOs.push_back(std::unique_ptr<DiscreteOperator>(new Grip(.0)));
        DOs.push_back(std::unique_ptr<DiscreteOperator>(new Ring(.0)));
        DOs.push_back(std::unique_ptr<DiscreteOperator>(new Empty(1.)));
        for (size_t i = 0; i != 5; ++i) {
            indexes[i] = i;
        }

    }
    discrete_vector(double p1, double p2, double p3, double p4) {
        try {
            if ((p1 + p2 + p3 + p4) > 1 + acc) {
                throw std::invalid_argument("ЗАДАННЫЕ ВЕРОЯТНОСТИ НЕ СООТВЕТСТВУЮТ ТРЕБОВАНИЯМ");
            }
        } catch (std::invalid_argument& e) {
            std::cerr << e.what() << '\n';
            std::cerr << "Сумма значений всех вероятностей не должна превышать единицу\n";
            exit(1);
        }

        DOs.reserve(5);
        DOs.push_back(std::unique_ptr<DiscreteOperator>(new Inverse(p1)));
        DOs.push_back(std::unique_ptr<DiscreteOperator>(new Symmetry(p2)));
        DOs.push_back(std::unique_ptr<DiscreteOperator>(new Grip(p3)));
        DOs.push_back(std::unique_ptr<DiscreteOperator>(new Ring(p4)));
        DOs.push_back(std::unique_ptr<DiscreteOperator>(new Empty(1. - (p1 + p2 + p3 + p4))));

        std::sort(DOs.begin(), DOs.end(),
            [](const std::unique_ptr<DiscreteOperator>& one,
                const std::unique_ptr<DiscreteOperator>& two) -> bool {
                return (one->GetProb() < two->GetProb());
            }
        );

        for (size_t i = 0; i != 5; ++i) {
            if (typeid(*DOs[i]) == typeid(Inverse)) { indexes[0] = i; }
            else if (typeid(*DOs[i]) == typeid(Symmetry)) { indexes[1] = i; }
            else if (typeid(*DOs[i]) == typeid(Grip)) { indexes[2] = i; }
            else if (typeid(*DOs[i]) == typeid(Ring)) { indexes[3] = i; }
            else { indexes[4] = i; }
        }
    }

    const std::unique_ptr<DiscreteOperator>& operator[] (size_t j) const { return DOs[j]; }
    void SetInverseProb(double prob) {
        // pi + pe = a
        // pi -> prob
        // prob + pe' = pi + pe = a
        //  pe' = pi + pe - prob
        double pi = DOs[indexes[0]]->GetProb();
        double pe = DOs[indexes[4]]->GetProb();

        try {
            if ((pi + pe - prob) < 0.) {
                throw std::invalid_argument("НЕВОЗМОЖНО ИЗМЕНИТЬ ВЕРОЯТНОСТЬ ОПЕРАТОРА ИНВЕРСИИ");
            }
        }
        catch (std::invalid_argument& e) {
            std::cerr << e.what() << '\n';
            std::cerr << "Сумма значений всех вероятностей не должна превышать единицу\n";
            exit(1);
        }

        DOs[indexes[0]]->SetProb(prob);
        DOs[indexes[4]]->SetProb(pi + pe - prob);
        std::sort(DOs.begin(), DOs.end(),
            [](const std::unique_ptr<DiscreteOperator>& one,
                const std::unique_ptr<DiscreteOperator>& two) -> bool {
                return (one->GetProb() < two->GetProb());
            }
        );

        for (size_t i = 0; i != 5; ++i) {
            if (typeid(*DOs[i]) == typeid(Inverse)) { indexes[0] = i; }
            else if (typeid(*DOs[i]) == typeid(Symmetry)) { indexes[1] = i; }
            else if (typeid(*DOs[i]) == typeid(Grip)) { indexes[2] = i; }
            else if (typeid(*DOs[i]) == typeid(Ring)) { indexes[3] = i; }
            else { indexes[4] = i; }
        }
    }
    void SetSymmetryProb(double prob) {
        double ps = DOs[indexes[1]]->GetProb();
        double pe = DOs[indexes[4]]->GetProb();

        try {
            if ((ps + pe - prob) < 0.) {
                throw std::invalid_argument("НЕВОЗМОЖНО ИЗМЕНИТЬ ВЕРОЯТНОСТЬ ОПЕРАТОРА СИММЕТРИИ");
            }
        }
        catch (std::invalid_argument& e) {
            std::cerr << e.what() << '\n';
            std::cerr << "Сумма значений всех вероятностей не должна превышать единицу\n";
            exit(1);
        }

        DOs[indexes[1]]->SetProb(prob);
        DOs[indexes[4]]->SetProb(ps + pe - prob);
        std::sort(DOs.begin(), DOs.end(),
            [](const std::unique_ptr<DiscreteOperator>& one,
                const std::unique_ptr<DiscreteOperator>& two) -> bool {
                return (one->GetProb() < two->GetProb());
            }
        );

        for (size_t i = 0; i != 5; ++i) {
            if (typeid(*DOs[i]) == typeid(Inverse)) { indexes[0] = i; }
            else if (typeid(*DOs[i]) == typeid(Symmetry)) { indexes[1] = i; }
            else if (typeid(*DOs[i]) == typeid(Grip)) { indexes[2] = i; }
            else if (typeid(*DOs[i]) == typeid(Ring)) { indexes[3] = i; }
            else { indexes[4] = i; }
        }
    }
    void SetGripProb(double prob) {
        double pg = DOs[indexes[2]]->GetProb();
        double pe = DOs[indexes[4]]->GetProb();

        try {
            if ((pg + pe - prob) < 0.) {
                throw std::invalid_argument("НЕВОЗМОЖНО ИЗМЕНИТЬ ВЕРОЯТНОСТЬ ОПЕРАТОРА СЖАТИЯ");
            }
        }
        catch (std::invalid_argument& e) {
            std::cerr << e.what() << '\n';
            std::cerr << "Сумма значений всех вероятностей не должна превышать единицу\n";
            exit(1);
        }

        DOs[indexes[2]]->SetProb(prob);
        DOs[indexes[4]]->SetProb(pg + pe - prob);
        std::sort(DOs.begin(), DOs.end(),
            [](const std::unique_ptr<DiscreteOperator>& one,
                const std::unique_ptr<DiscreteOperator>& two) -> bool {
                return (one->GetProb() < two->GetProb());
            }
        );

        for (size_t i = 0; i != 5; ++i) {
            if (typeid(*DOs[i]) == typeid(Inverse)) { indexes[0] = i; }
            else if (typeid(*DOs[i]) == typeid(Symmetry)) { indexes[1] = i; }
            else if (typeid(*DOs[i]) == typeid(Grip)) { indexes[2] = i; }
            else if (typeid(*DOs[i]) == typeid(Ring)) { indexes[3] = i; }
            else { indexes[4] = i; }
        }
    }
    void SetRingProb(double prob) {
        double pr = DOs[indexes[3]]->GetProb();
        double pe = DOs[indexes[4]]->GetProb();

        try {
            if ((pr + pe - prob) < 0.) {
                throw std::invalid_argument("НЕВОЗМОЖНО ИЗМЕНИТЬ ВЕРОЯТНОСТЬ ОПЕРАТОРА КОЛЬЦА");
            }
        }
        catch (std::invalid_argument& e) {
            std::cerr << e.what() << '\n';
            std::cerr << "Сумма значений всех вероятностей не должна превышать единицу\n";
            exit(1);
        }

        DOs[indexes[3]]->SetProb(prob);
        DOs[indexes[4]]->SetProb(pr + pe - prob);
        std::sort(DOs.begin(), DOs.end(),
            [](const std::unique_ptr<DiscreteOperator>& one,
                const std::unique_ptr<DiscreteOperator>& two) -> bool {
                return (one->GetProb() < two->GetProb());
            }
        );

        for (size_t i = 0; i != 5; ++i) {
            if (typeid(*DOs[i]) == typeid(Inverse)) { indexes[0] = i; }
            else if (typeid(*DOs[i]) == typeid(Symmetry)) { indexes[1] = i; }
            else if (typeid(*DOs[i]) == typeid(Grip)) { indexes[2] = i; }
            else if (typeid(*DOs[i]) == typeid(Ring)) { indexes[3] = i; }
            else { indexes[4] = i; }
        }
    }
    void SetProbs(double ps, double pi, double pg, double pr) {
        try {
            if (ps + pi + pg + pr > 1. + acc) {
                throw std::invalid_argument("НЕВАЛИДНЫЙ АРГУМЕНТ");
            }
        } catch (std::invalid_argument& e) {
            std::cerr << e.what() << '\n';
            std::cerr << "Сумма значений всех вероятностей не должна превышать единицу\n";
            exit(1);
        }

        DOs[indexes[0]]->SetProb(ps);
        DOs[indexes[1]]->SetProb(pi);
        DOs[indexes[2]]->SetProb(pg);
        DOs[indexes[3]]->SetProb(pr);
        DOs[indexes[4]]->SetProb(1. - ps - pi - pg - pr);
        std::sort(DOs.begin(), DOs.end(),
            [](const std::unique_ptr<DiscreteOperator>& one,
                const std::unique_ptr<DiscreteOperator>& two) -> bool {
                return (one->GetProb() < two->GetProb());
            }
        );

        for (size_t i = 0; i != 5; ++i) {
            if (typeid(*DOs[i]) == typeid(Inverse)) { indexes[0] = i; }
            else if (typeid(*DOs[i]) == typeid(Symmetry)) { indexes[1] = i; }
            else if (typeid(*DOs[i]) == typeid(Grip)) { indexes[2] = i; }
            else if (typeid(*DOs[i]) == typeid(Ring)) { indexes[3] = i; }
            else { indexes[4] = i; }
        }
    }
    void SetProbs(std::vector<double> probs) {
        try {
            if (probs.size() > 4) {
                throw std::invalid_argument("НЕВАЛИДНЫЙ АРГУМЕНТ");
            }
        }
        catch (std::invalid_argument& e) {
            std::cerr << e.what() << '\n';
            std::cerr << "Неверный размер переданного вектора\n";
            exit(1);
        }

        double s = 0;
        for (auto x : probs) { s += x;  }
        try {
            if (s > 1.+ acc) {
                throw std::invalid_argument("НЕВАЛИДНЫЙ АРГУМЕНТ");
            }
        }
        catch (std::invalid_argument& e) {
            std::cerr << e.what() << '\n';
            std::cerr << "Сумма значений всех вероятностей не должна превышать единицу\n";
            exit(1);
        }

        for (size_t i = 0; i != 5; ++i) {
            DOs[i]->SetProb(0.);
        }
        for (size_t i = 0; i != probs.size(); ++i) {
            DOs[indexes[i]]->SetProb(probs[i]);
        }

        DOs[indexes[4]]->SetProb(1. - s);
        std::sort(DOs.begin(), DOs.end(),
            [](const std::unique_ptr<DiscreteOperator>& one,
                const std::unique_ptr<DiscreteOperator>& two) -> bool {
                return (one->GetProb() < two->GetProb());
            }
        );

        for (size_t i = 0; i != 5; ++i) {
            if (typeid(*DOs[i]) == typeid(Inverse)) { indexes[0] = i; }
            else if (typeid(*DOs[i]) == typeid(Symmetry)) { indexes[1] = i; }
            else if (typeid(*DOs[i]) == typeid(Grip)) { indexes[2] = i; }
            else if (typeid(*DOs[i]) == typeid(Ring)) { indexes[3] = i; }
            else { indexes[4] = i; }
        }
    }
    size_t rand_choice() const {
        double rnd = gen.d_udist(0, 1);
        size_t i = 0;
        for (size_t i = 0; i < 5; ++i) {
            rnd -= DOs[i]->GetProb();
            if (rnd <= .0) return i;
        }

        return 4;
    }
};

// Второй вариант применения ДО к последовательности
// Применяем N раз какие-нибудь ДО к рандомным элементам последовательности
std::vector<int> gen_discrete_path(const Maze& mz,
    const discrete_vector& DV, const std::vector<int>& seq, size_t N) {

    std::vector<int> new_seq = seq;
    if (seq.size() < 3) { return seq; }

    for (size_t j = 0; j != N; ++j) {
        if (new_seq.size() >= treshold) {
            return new_seq;
        }

        int k = gen.int_udist(2, new_seq.size() - 2);
        std::vector<int> u(new_seq.begin(), new_seq.begin() + k);
        std::vector<int> v(new_seq.begin() + k, new_seq.end());

        size_t o_ind = DV.rand_choice();
        size_t LENGHT = gen.int_udist(2, 6);
        if (typeid(*DV[o_ind]) == typeid(Grip)) {
            LENGHT = (size_t)gen.int_udist(6, 30);
        }

        DV[o_ind]->method(mz, u, LENGHT);

        std::vector<int> bridge = mz.GetTensor()(u.back(), v.front());
        if (bridge.size() == 0) { continue; }
        for (size_t i = 0; i < bridge.size() - 1; ++i) {
            u.push_back(bridge[i]);
        }

        new_seq = u;
        new_seq.insert(new_seq.end(), v.begin(), v.end());
    }

    return new_seq;
}
//
//std::vector<std::vector<int>> gen_paths_file_int(const Maze& mz,
//            const discrete_vector& DOs, size_t trials, std::vector<int> seq, size_t N) {
//
//    std::ofstream file("sequences.txt", std::ios_base::out);
//    try {
//        if (!file.is_open())
//            throw std::runtime_error("НЕ УДАЛОСЬ ОТКРЫТЬ ИЛИ СОЗДАТЬ ФАЙЛ");
//    }
//    catch (std::runtime_error& e) {
//        std::cerr << e.what() << '\n';
//        exit(1);
//    }
//
//    auto tmp = int_to_char(mz.GetBjn(), seq);
//    for (auto it = tmp.begin(); it != tmp.end(); ++it) { file << *it; }
//    file << '\n';
//
//    std::vector<std::vector<int>> paths;
//    paths.reserve(trials + 1);
//    paths.push_back(seq);
//    for (size_t i = 0; i < trials; ++i) {
//        seq = gen_discrete_path(mz, DOs, seq, N);
//
//        paths.push_back(seq);
//        auto seq_chars = int_to_char(mz.GetBjn(), seq);
//        for (auto it = seq_chars.begin(); it != seq_chars.end(); ++it) { file << *it; }
//        file << '\n';
//    }
//
//     Последовательности будут разделены переносом строки
//    file << '\n';
//    file.close();
//
//    return paths;
//}

// Функция возвращает вектор последовательностей в числовом виде,
// Вместе с этим записывает из в файл в буквенном виде
// Файл всегда называется sequences.txt, при отсутствии он будет создан
std::vector<std::string> gen_paths_file_str(const Maze& mz,
    const discrete_vector& DOs, size_t trials, std::vector<int> seq, size_t N) {

    std::ofstream file("sequences.txt", std::ios_base::out | std::ios_base::app);
    try {
        if (!file.is_open())
            throw std::runtime_error("НЕ УДАЛОСЬ ОТКРЫТЬ ИЛИ СОЗДАТЬ ФАЙЛ");
    }
    catch (std::runtime_error& e) {
        std::cerr << e.what() << '\n';
        exit(1);
    }

    auto tmp = int_to_char_str(mz.GetBjn(), seq);
    for (auto it = tmp.begin(); it != tmp.end(); ++it) { file << *it; }
    file << '\n';

    std::vector<std::string> paths;
    paths.reserve(trials + 1);
    paths.push_back(tmp);
    for (size_t i = 0; i < trials; ++i) {
        seq = gen_discrete_path(mz, DOs, seq, N);
        std::string seq_chars = int_to_char_str(mz.GetBjn(), seq);
        paths.push_back(seq_chars);
        for (auto it = seq_chars.begin(); it != seq_chars.end(); ++it) { file << *it; }
        file << '\n';
    }

    // Последовательности будут разделены переносом строки
    file << '\n';
    file.close();

    return paths;
}