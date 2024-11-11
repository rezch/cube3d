#include <iostream>


template <class... Args>
void write(Args&&... args) {
    ((std::cout << args << ' '), ...) << std::endl;
};

void write() {
    std::cout << std::endl;
}


template <class... Args>
void read(Args&&... args) {
    ((std::cin >> args), ...);
};

