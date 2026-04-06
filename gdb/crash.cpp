// crash.cpp
#include <iostream>


int boom(int* p) {
    int g = 9;
    return *p; // segfault when p == nullptr
}

int compute() {
    int* x = nullptr;
    return boom(x);
}

int main() {
    int v = compute();
    std::cout << "v=" << v << '\n';
    return 0;
}
