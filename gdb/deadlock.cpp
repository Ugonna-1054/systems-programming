// deadlock.cpp
#include <chrono>
#include <iostream>
#include <mutex>
#include <thread>

std::mutex m1;
std::mutex m2;

void t1_fn() {
    std::lock_guard<std::mutex> lk1(m1);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    std::lock_guard<std::mutex> lk2(m2); // waits if t2 holds m2
    std::cout << "t1 done\n";
}

void t2_fn() {
    std::lock_guard<std::mutex> lk1(m2);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    std::lock_guard<std::mutex> lk2(m1); // waits if t1 holds m1
    std::cout << "t2 done\n";
}

int main() {
    std::thread t1(t1_fn);
    std::thread t2(t2_fn);

    t1.join();
    t2.join();

    std::cout << "main done\n";
    return 0;
}
