#pragma once
#include <chrono>
#include <iostream>

struct Benchmark
{
private:
    static std::chrono::steady_clock::time_point t1;

public:
    static void Start()
    {
        t1 = std::chrono::steady_clock::now();
    }

    static void End(const char* test)
    {
        std::cout << test << " | Duration : " << std::chrono::duration_cast<std::chrono::milliseconds>((std::chrono::steady_clock::now() - t1)).count() << "ms" << std::endl;
    }
};
