#pragma once

#include <iostream>


template <class... Args>
inline void write(Args&&... args) {
    ((std::cout << args << ' '), ...) << std::endl;
};

inline void write() {
    std::cout << std::endl;
}


template <class... Args>
inline void read(Args&&... args) {
    ((std::cin >> args), ...);
};

