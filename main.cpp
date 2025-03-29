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
#include <iomanip>
#include <random>
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
    double result = std::sin(value);
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
            else {
                sum += numbers[size/2];
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
            else {
                sum += numbers[size/2];
            }
        }
    }
    timer.stop();
    return timer.duration<zen::timer::nsec>().count() / 1e9;
}

auto run_unsorted_predictable_complex(const std::vector<int>& numbers, int iter, int size, volatile double& sum) {
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
            else {
                zen::timer inner_timer;
                inner_timer.start();
                sum += complex_process(numbers[size/2]);
                total_complex_time += inner_timer.elapsed<zen::timer::nsec>().count();
            }
        }
    }
    timer.stop();
    return (timer.duration<zen::timer::nsec>().count() - total_complex_time) / 1e9;
}

auto run_unsorted_unpredictable_complex(const std::vector<int>& numbers, int iter, int size, volatile double& sum) {
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
            else {
                zen::timer inner_timer;
                inner_timer.start();
                sum += complex_process(numbers[size/2]);
                total_complex_time += inner_timer.elapsed<zen::timer::nsec>().count();
            }
        }
    }
    timer.stop();
    return (timer.duration<zen::timer::nsec>().count() - total_complex_time) / 1e9;
}

// Sorted test cases
auto run_sorted_unpredictable(std::vector<int>& numbers, int iter, int size, volatile auto& sum) {
    std::sort(numbers.begin(), numbers.end());
    zen::timer timer;
    timer.start();
    for (int i = 0; i < iter; i++) {
        for (int j = 0; j < size; j++) {
            if (zen::random_int(0, size) > numbers[j]) {
                sum += numbers[j];
            }
            else {
                sum += numbers[size/2];
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
            else {
                sum += numbers[size/2];
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
            else {
                zen::timer inner_timer;
                inner_timer.start();
                sum += complex_process(numbers[size/2]);
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
            else {
                zen::timer inner_timer;
                inner_timer.start();
                sum += complex_process(numbers[size/2]);
                total_complex_time += inner_timer.elapsed<zen::timer::nsec>().count();
            }
        }
    }
    timer.stop();
    return (timer.duration<zen::timer::nsec>().count() - total_complex_time) / 1e9;
}

int main(int argc, char* argv[]) {
    auto [size, iter] = process_args(argc, argv);
    std::vector<int> numbers(size);
    volatile double sum = 0;

    // Generate test data once
    for (int i = 0; i < size; i++) {
        numbers[i] = zen::random_int(0, size);
    }

    // Warm-up to stabilize CPU state
    warm_up(sum, size);

    // Pretty table header
    zen::print("\n" ,std::format("{:=^66}\n", " Branch Prediction Timing Results "));
    zen::print(std::format("  Size: {:<6} | Iterations: {}\n", size, iter));
    zen::print(std::format("| {:<36} | {:>12} | {:<9} |\n", "Test Case", "Time (s)", "Unit"));
    zen::print(std::format("{:-<67}\n", ""));
    zen::print(std::format("| {:^36} | {:>12} | {:<9} |\n", "Unsorted Data", "", ""));

    // Unsorted tests
    double unpredictable_time = run_unsorted_unpredictable(numbers, iter, size, sum);
    zen::print(zen::color::red(std::format("| {:<36} | {:>12.6f} | {:<9} |\n", "Unpredictable", unpredictable_time, "seconds")));

    double predictable_time = run_unsorted_predictable(numbers, iter, size, sum);
    zen::print(zen::color::green(std::format("| {:<36} | {:>12.6f} | {:<9} |\n", "Predictable", predictable_time, "seconds")));

    double percent_diff_unsorted = ((unpredictable_time - predictable_time) / unpredictable_time) * 100;
    zen::print(std::format("| {:<36} | {:>12.2f} | {:<9} |\n", "Percent Difference (Unpred - Pred)", percent_diff_unsorted, "%"));

    double predictable_complex_time = run_unsorted_predictable_complex(numbers, iter, size, sum);
    zen::print(zen::color::green(std::format("| {:<36} | {:>12.6f} | {:<9} |\n", "Predictable Complex", predictable_complex_time, "seconds")));

    double unpredictable_complex_time = run_unsorted_unpredictable_complex(numbers, iter, size, sum);
    zen::print(zen::color::red(std::format("| {:<36} | {:>12.6f} | {:<9} |\n", "Unpredictable Complex", unpredictable_complex_time, "seconds")));

    double percent_diff_complex_unsorted = ((unpredictable_complex_time - predictable_complex_time) / unpredictable_complex_time) * 100;
    zen::print(std::format("| {:<36} | {:>12.2f} | {:<9} |\n", "Percent Difference (Unpred - Pred)", percent_diff_complex_unsorted, "%"));

    // Separator for sorted section
    zen::print(std::format("{:-<67}\n", ""));
    zen::print(std::format("| {:^36} | {:>12} | {:<9} |\n", "Sorted Data", "", ""));

    // Sorted tests (using copies to preserve original unsorted data)
    std::vector<int> numbers_sorted = numbers; // Copy for sorted tests
    double sorted_unpredictable_time = run_sorted_unpredictable(numbers_sorted, iter, size, sum);
    zen::print(zen::color::red(std::format("| {:<36} | {:>12.6f} | {:<9} |\n", "Unpredictable", sorted_unpredictable_time, "seconds")));

    numbers_sorted = numbers; // Reset for next test
    double sorted_predictable_time = run_sorted_predictable(numbers_sorted, iter, size, sum);
    zen::print(zen::color::green(std::format("| {:<36} | {:>12.6f} | {:<9} |\n", "Predictable", sorted_predictable_time, "seconds")));

    double percent_diff_sorted = ((sorted_unpredictable_time - sorted_predictable_time) / sorted_unpredictable_time) * 100;
    zen::print(std::format("| {:<36} | {:>12.2f} | {:<9} |\n", "Percent Difference (Unpred - Pred)", percent_diff_sorted, "%"));

    numbers_sorted = numbers; // Reset for next test
    double sorted_predictable_complex_time = run_sorted_predictable_complex(numbers_sorted, iter, size, sum);
    zen::print(zen::color::green(std::format("| {:<36} | {:>12.6f} | {:<9} |\n", "Predictable Complex", sorted_predictable_complex_time, "seconds")));

    numbers_sorted = numbers; // Reset for next test
    double sorted_unpredictable_complex_time = run_sorted_unpredictable_complex(numbers_sorted, iter, size, sum);
    zen::print(zen::color::red(std::format("| {:<36} | {:>12.6f} | {:<9} |\n", "Unpredictable Complex", sorted_unpredictable_complex_time, "seconds")));

    double percent_diff_complex_sorted = ((sorted_unpredictable_complex_time - sorted_predictable_complex_time) / sorted_unpredictable_complex_time) * 100;
    zen::print(std::format("| {:<36} | {:>12.2f} | {:<9} |\n", "Percent Difference (Unpred - Pred)", percent_diff_complex_sorted, "%"));

    // Final comparisons
    zen::print(std::format("{:-<67}\n", ""));
    zen::print(std::format("| {:^36} | {:>12}  {:<10} |\n", "Unsorted vs Sorted Data", "", ""));
    double sorted_vs_not_sorted = ((predictable_time - sorted_predictable_time) / predictable_time) * 100;
    zen::print(std::format("| {:<36} | {:>12.2f} | {:<9} |\n", "Percent Difference", sorted_vs_not_sorted, "%"));

    double sorted_vs_not_sorted_complex = ((predictable_complex_time - sorted_predictable_complex_time) / predictable_complex_time) * 100;
    zen::print(std::format("| {:<36} | {:>12.2f} | {:<9} |\n", "Percent Difference complex", sorted_vs_not_sorted_complex, "%"));

    zen::print(std::format("{:-<67}\n", ""));
    return 0;
}