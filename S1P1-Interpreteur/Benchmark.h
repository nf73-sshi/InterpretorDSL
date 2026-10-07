#pragma once
#include <chrono>
#include <iostream>

struct Benchmark
{
private:
    static std::chrono::high_resolution_clock::time_point t1;

public:
    static void Start()
    {
        t1 = std::chrono::high_resolution_clock::now();
    }

    static void End(const char* test)
    {
        std::cout << test << " | Duration : " << std::chrono::high_resolution_clock::now() - t1 << std::endl;
    }
};

std::chrono::high_resolution_clock::time_point Benchmark::t1 = {};
