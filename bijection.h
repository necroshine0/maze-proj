#pragma once
#include <vector>
#include <unordered_map>
#include <algorithm>

class Bjn {
private:
    std::unordered_map<char, int> digs;
    std::unordered_map<int, char> symbs;

public:
    Bjn() {}
    Bjn(std::vector<char> chars) {
        std::sort(chars.begin(), chars.end());
        for (size_t i = 0; i != chars.size(); ++i) {
            digs[chars[i]] = i;
            symbs[i] = chars[i];

        }
    }

    void numbered_chars() const {
        size_t n = digs.size();
        for (size_t i = 0; i != n; ++i) {
            std::cout << symbs.at(i) << ":" << i << "; ";
        }
        std::cout << '\n';
    }

    int operator[] (char c) const { return this->digs.at(c); }
    template <typename T>
    char operator[] (T v) const { return this->symbs.at(v); }

    size_t size() const { return digs.size(); }

    friend std::vector<int> char_to_int(const Bjn& bjn,
        const std::vector<char>& path_char);
    friend std::vector<char> int_to_char(const Bjn& bjn,
        const std::vector<int>& path_int);
};

std::vector<int> char_to_int(const Bjn& bjn, const std::vector<char>& path_char) {
    std::vector<int> path_int(path_char.size());
    for (size_t i = 0; i != path_int.size(); ++i) {
        path_int[i] = bjn.digs.at(path_char[i]);
    }

    return path_int;
}

std::vector<char> int_to_char(const Bjn& bjn, const std::vector<int>& path_int) {
    std::vector<char> path_char(path_int.size());
    for (size_t i = 0; i != path_int.size(); ++i) {
        path_char[i] = bjn.symbs.at(path_int[i]);
    }

    return path_char;
}
