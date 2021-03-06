#pragma once
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <string>

class Bjn {
private:
    std::unordered_map<wchar_t, int> digs;
    std::unordered_map<int, wchar_t> symbs;

public:
    Bjn() {}
    Bjn(std::vector<wchar_t> chars) {
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

    int operator[] (wchar_t c) const { return this->digs.at(c); }
    template <typename T>
    wchar_t operator[] (T v) const { return this->symbs.at(v); }

    size_t size() const { return digs.size(); }

    friend std::vector<int> char_to_int(const Bjn& bjn,
        const std::vector<wchar_t>& path_char);
    friend std::vector<int> char_to_int(const Bjn& bjn,
        const std::wstring& path_char);
    friend std::vector<wchar_t> int_to_char(const Bjn& bjn,
        const std::vector<int>& path_int);
    friend std::wstring int_to_char_str(const Bjn& bjn,
        const std::vector<int>& path_int);
};

std::vector<int> char_to_int(const Bjn& bjn, const std::vector<wchar_t>& path_char) {
    std::vector<int> path_int(path_char.size());
    for (size_t i = 0; i != path_int.size(); ++i) {
        path_int[i] = bjn.digs.at(path_char[i]);
    }

    return path_int;
}

std::vector<int> char_to_int(const Bjn& bjn, const std::wstring& path_char) {
    std::vector<int> path_int(path_char.size());
    for (size_t i = 0; i != path_int.size(); ++i) {
        path_int[i] = bjn.digs.at(path_char[i]);
    }

    return path_int;
}

std::vector<wchar_t> int_to_char(const Bjn& bjn, const std::vector<int>& path_int) {
    std::vector<wchar_t> path_char(path_int.size());
    for (size_t i = 0; i != path_int.size(); ++i) {
        path_char[i] = bjn.symbs.at(path_int[i]);
    }

    return path_char;
}

std::wstring int_to_char_str(const Bjn& bjn, const std::vector<int>& path_int) {
    std::wstring path_char;
    path_char.resize(path_int.size());
    for (size_t i = 0; i != path_int.size(); ++i) {
        path_char[i] = bjn.symbs.at(path_int[i]);
    }

    return path_char;
}