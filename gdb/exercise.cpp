// deadlock.cpp
#include <chrono>
#include <iostream>
#include <mutex>
#include <thread>
#include <unistd.h>

std::mutex m1;
std::mutex m2;

// int counter = 0;
int N = 1'000'000;
std::atomic<long> counter;

void t1_fn() {
    for (int i=0; i<N; i++) {
        counter.fetch_add(1, std::memory_order_relaxed);
    }
    std::cout << "t1 done\n";
}

void t2_fn() {
     for (int i=0; i<N; i++) {
        counter.fetch_add(1, std::memory_order_relaxed);
    }
    std::cout << "t2 done\n";
}

void t3_fn() {
    for (int i=0; i<N; i++) {
        counter.fetch_add(1, std::memory_order_relaxed);
    }
    std::cout << "t3 done\n";
}

void print() {
    while(1) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        std::cout << "counter is " << counter.load(std::memory_order_relaxed) << std::endl;
    }
}

int main() {

    std::cout << getpid() << "\n";
    
    std::jthread t1(t1_fn);
    std::jthread t2(t2_fn);
    std::jthread t3(t3_fn);
    std::jthread t4(print);


    std::cout << "main done\n";
    return 0;
}
