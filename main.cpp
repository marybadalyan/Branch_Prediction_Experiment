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

// not using zen::timer for smaller duration because it returns negative numbers
struct Timer {
    std::chrono::high_resolution_clock::time_point start_time;
    
    void start() {
        start_time = std::chrono::high_resolution_clock::now();
    }
    
    double duration() const {  // Returns seconds
        auto end_time = std::chrono::high_resolution_clock::now();
        return std::chrono::duration_cast<std::chrono::nanoseconds>(end_time - start_time).count() / 1e9;
    }
};

std::pair<int,int> process_args(int argc, char* argv[]) {
    zen::cmd_args args(argv, argc);
    auto size_options = args.get_options("--size");
    auto iter_options = args.get_options("--iter");

    if (size_options.empty() || iter_options.empty()) {
        std::cout << "Error: --size and/or --iter arguments are absent, using default 1000!" << std::endl;
        return {1000,1000};
    }
    return {std::stoi(size_options[0]), std::stoi(iter_options[0])};
}

double complex_process(int value) {
    double result = value;
    for (int i = 0; i < 100; i++) {  // Simulate heavy work
        result += std::sin(result) * std::cos(result);
    }
    return result;
}

double complex_process_time(int value) {
    Timer timer;
    timer.start();
    complex_process(value);  // Time the function execution to subtract 
    return timer.duration(); // Return the time taken to execute
}

int main(int argc, char* argv[]) {
    auto [size, iter] = process_args(argc, argv);
    std::vector<int> numbers(size);
    std::vector<int> random_conditions(size);
    volatile double sum = 0;  // Changed to double for complex_process
    Timer timer;

    // Generate test data once
    for (int i = 0; i < size; i++) {
        numbers[i] = zen::random_int(0,size);
        random_conditions[i] = zen::random_int(0,size);
    }

    // Table header
    std::cout << "\n" << std::format("{:=^60}\n", " Branch Prediction Timing Results ");
    std::cout << std::format("Size: {:<6} | Iterations: {}\n", size, iter);
    std::cout << std::format("{:-<60}\n", "");
    std::cout << std::format("| {:<30} | {:>12} | {:<9} |\n", "Test Case", "Time", "Unit");
    std::cout << std::format("|{:-<32}|{:-<14}|{:-<11}|\n", "", "", "");

    // Unpredictable (unsorted)
    double unpredictable_time;
    timer.start();
    for (int i = 0; i < iter; i++) {
        for (int j = 0; j < size; j++) {
            if (random_conditions[j] > numbers[j]) {
                sum += numbers[j];
            }
        }
    }
    unpredictable_time = timer.duration();
    std::cout << std::format("| {:<30} | {:>12.6f} | {:<9} |\n", "Unpredictable (unsorted)", unpredictable_time, "seconds");

    // Predictable (unsorted)
    double predictable_time;
    timer.start();
    for (int i = 0; i < iter; i++) {
        for (int j = 0; j < size; j++) {
            if (500 > numbers[j]) {
                sum += numbers[j];
            }
        }
    }
    predictable_time = timer.duration();
    std::cout << std::format("| {:<30} | {:>12.6f} | {:<9} |\n", "Predictable (unsorted)", predictable_time, "seconds");

    // Predictable complex process (unsorted) - Remove execution time of the function
    double predictable_complex_time;
    timer.start();
    double total_complex_time = 0.0; // Track total time spent in the complex process
    for (int i = 0; i < iter; i++) {
        for (int j = 0; j < size; j++) {
            if (500 > numbers[j]) {
                total_complex_time += complex_process_time(numbers[j]); // Track time for each complex call
            }
        }
    }
    predictable_complex_time = timer.duration() - total_complex_time; // Subtract function execution time
    std::cout << std::format("| {:<30} | {:>12.6f} | {:<9} |\n", "Predictable complex (unsorted)", predictable_complex_time, "seconds");


    // Speedup factor (unsorted)
    double speedup_factor_unsorted = unpredictable_time / predictable_time;
    std::cout << std::format("| {:<30} | {:>12.6f} | {:<9} |\n", "Speedup factor (unsorted)", speedup_factor_unsorted, "");

    // Separator
    std::cout << std::format("|{:-<32}|{:-<14}|{:-<11}|\n", "", "", "");

  
    std::sort(numbers.begin(), numbers.end());
    // Unpredictable (sorted)
    double sorted_unpredictable_time;
    timer.start();
    for (int i = 0; i < iter; i++) {
        for (int j = 0; j < size; j++) {
            if (random_conditions[j] > numbers[j]) {
                sum += numbers[j];
            }
        }
    }
    sorted_unpredictable_time = timer.duration();
    std::cout << std::format("| {:<30} | {:>12.6f} | {:<9} |\n", "Unpredictable (sorted)", sorted_unpredictable_time, "seconds");

    // Predictable (sorted)
    double sorted_predictable_time;
    timer.start();
    for (int i = 0; i < iter; i++) {
        for (int j = 0; j < size; j++) {
            if (500 > numbers[j]) {
                sum += numbers[j];
            }
        }
    }
    sorted_predictable_time = timer.duration();
    std::cout << std::format("| {:<30} | {:>12.6f} | {:<9} |\n", "Predictable (sorted)", sorted_predictable_time, "seconds");

    // Predictable complex process (sorted) - Remove execution time of the function
    double sorted_predictable_complex_time;
    timer.start();
    total_complex_time = 0.0; // Reset total complex time for sorted
    for (int i = 0; i < iter; i++) {
        for (int j = 0; j < size; j++) {
            if (500 > numbers[j]) {
                total_complex_time += complex_process_time(numbers[j]); // Track time for each complex call
            }
        }
    }
    sorted_predictable_complex_time = timer.duration() - total_complex_time; // Subtract function execution time
    std::cout << std::format("| {:<30} | {:>12.6f} | {:<9} |\n", "Predictable complex (sorted)", sorted_predictable_complex_time, "seconds");
    
    double sorted_unpredictable_complex_time;
    timer.start();
    total_complex_time = 0.0; // Reset total complex time for sorted
    for (int i = 0; i < iter; i++) {
        for (int j = 0; j < size; j++) {
            if (random_conditions[j] > numbers[j]) {
                total_complex_time += complex_process_time(numbers[j]); // Track time for each complex call
            }
        }
    }
    sorted_unpredictable_complex_time = timer.duration() - total_complex_time; // Subtract function execution time
    std::cout << std::format("| {:<30} | {:>12.6f} | {:<9} |\n", "UnPredictable complex (sorted)", sorted_unpredictable_complex_time, "seconds");

    // Speedup factor (sorted)
    double speedup_factor_sorted = sorted_unpredictable_time / sorted_predictable_time;
    std::cout << std::format("| {:<30} | {:>12.6f} | {:<9} |\n", "Speedup factor (sorted)", speedup_factor_sorted, "");

    // Footer
    std::cout << std::format("{:-<61}\n", "");
    std::cout << "Note: Speedup factor > 1 means unpredictable is slower.\n";

    return 0;
}
