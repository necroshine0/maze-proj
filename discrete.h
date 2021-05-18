#pragma once
#include <iostream>
#include <fstream>
#include <vector>
#include <memory>
#include <algorithm>
#include "help_functions.h"
#include "tensor.h"
#include "maze.h"

extern RandomGenerator gen;

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

        std::vector<int> subv = subseq(v, v.size() - subseq_size, v.size());
        Directions DIRS = mz.GetDirections();
        auto str_v = DIRS.convert_to_dirs(subv);
        int first_vert = subv.front();
        // Теоретически можно использовать и горизонтальный тип разреза
        std::string type = "vertical";

        auto gph = mz.GetGraphList();
        auto vs = mz.GetVertexes();
        auto borders = mz.GetBorders(type);
        // std::cout << borders.first << ' ' << borders.second << '\n';
        // std::cout << vs[first_vert].GetX() << ' ' << vs[first_vert].GetY() << '\n';
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
        for (size_t i = 0; i < tmp_v.size() - 1; ++i) { v.push_back(tmp_v[i]); }
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
        if (subseq_size > v.size()) { return; }

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

        auto backup = v;
        std::vector<Vertex> VS = mz.GetVertexes();
        int fixed_v = *(--v.end());
        graph_int gph = mz.GetGraphList();
        int current_v = fixed_v;

        while (true) {
            size_t u = random_choice(gph[current_v]);
            current_v = gph[current_v][u];
            v.push_back(current_v);

            if (VS[current_v].bio_value) { 
                break;
            }
        }

        auto path = mz.GetTensor()(current_v, fixed_v);

        for (size_t i = 0; i < path.size(); ++i) {
            v.push_back(path[i]);
        }

        if (v.size() > 2 * gph.size() / 3) {
            v = backup;
            backup.clear();
            path.clear();
            gph.clear();
            VS.clear();
            method(mz, v, subseq_size);
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
            if ((p1 + p2 + p3 + p4) > 1) {
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
            if (ps + pi + pg + pr > 1.) {
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
            if (s > 1.) {
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

std::vector<char> gen_discrete_path(const Maze& mz,
        const discrete_vector& DV,
            const std::vector<int>& seq) {

    std::vector<int> new_seq;
    new_seq.reserve(seq.size());
    for (size_t i = 0; i < seq.size() - 1; ++i) {
        new_seq.push_back(seq[i]);
        if (new_seq.size() < 3) { continue; }
        size_t k = DV.rand_choice();

        size_t LENGHT = std::min(int(new_seq.size()), gen.int_udist(2, 6));

        DV[k]->method(mz, new_seq, LENGHT);

        // Посмотрим на последнюю полученную вершину
        // Если есть ребро с i+1-ой, то ок
        // Иначе нам надо прийти в i+1, 
        // желательно по кратчайшему пути
        // В целом, можно это и сделать и без проверки существования ребра
        std::vector<int> bridge = mz.GetTensor()(new_seq.back(), seq[i + 1]);
        
        if (bridge.size() == 0) { continue; }
        for (size_t j = 0; j < bridge.size() - 1; ++j) {
            new_seq.push_back(bridge[j]);
        }
    }

    new_seq.push_back(seq.back());
    return int_to_char(mz.GetBjn(), new_seq);
}

void gen_paths_file(const std::string& filename, const Maze& mz,
    const discrete_vector& DOs, size_t trials, const std::vector<int>& seq) {

    std::ofstream file(filename, std::ios_base::trunc | std::ios_base::out);
    try {
        if (!file.is_open())
            throw std::runtime_error("НЕ УДАЛОСЬ ОТКРЫТЬ ФАЙЛ");
    } catch (std::runtime_error& e) {
        std::cerr << e.what() << '\n';
        exit(1);
    }

    auto tmp = int_to_char(mz.GetBjn(), seq);
    for (auto it = tmp.begin(); it != tmp.end(); ++it) { file << *it; }
    file << '\n';

    for (size_t i = 0; i < trials; ++i) {
        auto v = gen_discrete_path(mz, DOs, seq);
        for (auto it = v.begin(); it != v.end(); ++it) { file << *it; }
        file << '\n';
    }

    file.close();
}
