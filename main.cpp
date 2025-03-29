#include <iostream>
#include <cstring>
#include <utility>
#include <cmath>
#include <algorithm>
#include <vector>
#include <chrono>
#include <random>
#include <format>
#include "kaizen.h"

// Parse command-line arguments
std::pair<int, int> process_args(int argc, char* argv[]) {
    zen::cmd_args args(argv, argc);
    auto size_options = args.get_options("--size");
    auto iter_options = args.get_options("--iter");

    if (size_options.empty() || iter_options.empty()) {
        zen::log("Error: --size and/or --iter arguments are absent, using default 1000!");
        return {1000, 1000};
    }
    return {std::stoi(size_options[0]), std::stoi(iter_options[0])};
}

// Simulate a computationally expensive function
double complex_process(int value) {
    double result = value;
    for (int i = 0; i < 100; i++) {  // Simulate heavy work
        result += std::sin(result) * std::cos(result);
    }
    return result;
}

// Warm-up function to stabilize CPU state
void warm_up(volatile double& sum, int size) {
    for (int i = 0; i < size; i++) {
        sum += i;
    }
}

// Unsorted test cases
auto run_unsorted_unpredictable(const std::vector<int>& numbers, int iter, int size, volatile double& sum) {
    zen::timer timer;
    timer.start();
    for (int i = 0; i < iter; i++) {
        for (int j = 0; j < size; j++) {
            if (zen::random_int(0, size) > numbers[j]) {
                sum += numbers[j];
            }
        }
    }
    timer.stop();
    return timer.duration<zen::timer::nsec>().count() / 1e9;
}

auto run_unsorted_predictable(const std::vector<int>& numbers, int iter, int size, volatile double& sum) {
    zen::timer timer;
    timer.start();
    for (int i = 0; i < iter; i++) {
        for (int j = 0; j < size; j++) {
            if (size/2 > numbers[j]) {
                sum += numbers[j];
            }
        }
    }
    timer.stop();
    return timer.duration<zen::timer::nsec>().count() / 1e9;
}

double run_unsorted_predictable_complex(const std::vector<int>& numbers, int iter, int size, volatile double& sum) {
    double total_complex_time = 0.0;
    zen::timer timer;
    timer.start();
    for (int i = 0; i < iter; i++) {
        for (int j = 0; j < size; j++) {
            if (size/2 > numbers[j]) {
                zen::timer inner_timer;
                inner_timer.start();
                sum += complex_process(numbers[j]);
                total_complex_time += inner_timer.elapsed<zen::timer::nsec>().count();
            }
        }
    }
    timer.stop();
    return (timer.duration<zen::timer::nsec>().count() - total_complex_time) / 1e9;
}

auto run_unsorted_unpredictable_complex(const std::vector<int>& numbers, int iter, int size, volatile double& sum) {
    double total_complex_time = 0.0;
    zen::timer timer;
    timer.start();
    for (int i = 0; i < iter; i++) {
        for (int j = 0; j < size; j++) {
            if (zen::random_int(0, size) > numbers[j]) {
                zen::timer inner_timer;
                inner_timer.start();
                sum += complex_process(numbers[j]);
                total_complex_time += inner_timer.elapsed<zen::timer::nsec>().count();
            }
        }
    }
    timer.stop();
    return (timer.duration<zen::timer::nsec>().count() - total_complex_time) / 1e9;
}

// Sorted test cases
double run_sorted_unpredictable(std::vector<int>& numbers, int iter, int size, volatile auto& sum) {
    std::sort(numbers.begin(), numbers.end());
    zen::timer timer;
    timer.start();
    for (int i = 0; i < iter; i++) {
        for (int j = 0; j < size; j++) {
            if (zen::random_int(0, size) > numbers[j]) {
                sum += numbers[j];
            }
        }
    }
    timer.stop();
    return timer.duration<zen::timer::nsec>().count() / 1e9;
}
auto run_sorted_predictable(std::vector<int>& numbers, int iter, int size, volatile auto& sum) {
    std::sort(numbers.begin(), numbers.end());
    zen::timer timer;
    timer.start();
    for (int i = 0; i < iter; i++) {
        for (int j = 0; j < size; j++) {
            if (size/2 > numbers[j]) {
                sum += numbers[j];
            }
        }
    }
    timer.stop();
    return timer.duration<zen::timer::nsec>().count() / 1e9;
}

auto run_sorted_predictable_complex(std::vector<int>& numbers, int iter, int size, volatile auto& sum) {
    std::sort(numbers.begin(), numbers.end());
    auto total_complex_time = 0.0;
    zen::timer timer;
    timer.start();
    for (int i = 0; i < iter; i++) {
        for (int j = 0; j < size; j++) {
            if (size/2 > numbers[j]) {
                zen::timer inner_timer;
                inner_timer.start();
                sum += complex_process(numbers[j]);
                total_complex_time += inner_timer.elapsed<zen::timer::nsec>().count();
            }
        }
    }
    timer.stop();
    return (timer.duration<zen::timer::nsec>().count() - total_complex_time) / 1e9;
}

auto run_sorted_unpredictable_complex(std::vector<int>& numbers, int iter, int size, volatile auto& sum) {
    std::sort(numbers.begin(), numbers.end());
    auto total_complex_time = 0.0;
    zen::timer timer;
    timer.start();
    for (int i = 0; i < iter; i++) {
        for (int j = 0; j < size; j++) {
            if (zen::random_int(0, size) > numbers[j]) {
                zen::timer inner_timer;
                inner_timer.start();
                sum += complex_process(numbers[j]);
                total_complex_time += inner_timer.elapsed<zen::timer::nsec>().count();
            }
        }
    }
    timer.stop();
    return (timer.duration<zen::timer::nsec>().count() - total_complex_time) / 1e9;
}
