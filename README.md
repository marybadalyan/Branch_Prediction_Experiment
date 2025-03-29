
# Branch Prediction Experiment

This project benchmarks the impact of branch prediction on performance by comparing predictable versus unpredictable branching conditions with sorted and unsorted data. It investigates how CPU branch prediction tables (BPT) handle varying branching patterns and workloads, ranging from simple comparisons to complex computations.

## Dependencies

- C++20 compiler (for `std::format`)
- Standard Template Library (STL)
- Custom headers:
  - [`kaizen.h`](https://github.com/heinsaar/kaizen) (provides `random_int`, `print`, `color` and other utilities)

## Build Instructions

1. **Clone the repository**:
    ```bash
    git clone https://github.com/marybadalyan/Branch_Prediction_Experiment
    ```

2. **Navigate to the repository**:
    ```bash
    cd Branch_Prediction_Experiment
    ```

3. **Generate build files**:
    ```bash
    cmake -DCMAKE_BUILD_TYPE=Release -S . -B build
    ```

4. **Build the project**:
    ```bash
    cmake --build build --config Release
    ```

5. **Run the executable** from the build directory:
    ```bash
    ./build/Branch_Prediction_Experiment
    ```

## Usage

Run the program with optional arguments to customize the vector size and number of iterations:

```bash
./Branch_Prediction_Experiment --size [num] --iter [num]
```

- `--size`: Number of elements in the vector (default: 1000).
- `--iter`: Number of iterations for each test (default: 1000).

## Example Output

Below is sample output from running the program with `--size 15000 --iter 5000`:

```
 ================ Branch Prediction Timing Results ================
  Size: 15000  | Iterations: 5000
| Test Case                            |     Time (s) | Unit      |
-------------------------------------------------------------------
|            Unsorted Data             |              |           |
| Unpredictable                        |     0.700319 | seconds   |
| Predictable                          |     0.200068 | seconds   |
| Percent Difference (Unpred - Pred)   |        71.43 | %         |
| Predictable Complex                  |     3.630773 | seconds   |
| Unpredictable Complex                |     4.097937 | seconds   |
| Percent Difference (Unpred - Pred)   |        11.40 | %         |
-------------------------------------------------------------------
|             Sorted Data              |              |           |
| Unpredictable                        |     0.520010 | seconds   |
| Predictable                          |     0.129374 | seconds   |
| Percent Difference (Unpred - Pred)   |        75.12 | %         |
| Predictable Complex                  |     3.661655 | seconds   |
| Unpredictable Complex                |     4.184143 | seconds   |
| Percent Difference (Unpred - Pred)   |        12.49 | %         |
-------------------------------------------------------------------
|       Unsorted vs Sorted Data        |                          |
| Percent Difference                   |        35.34 | %         |
| Percent Difference Complex           |        -0.85 | %         |
-------------------------------------------------------------------
```

## Observations and Analysis

### What is Branch Prediction?
Branch prediction is a CPU optimization technique that predicts the outcome of conditional branches (e.g., `if` statements) to keep the instruction pipeline full. Correct predictions avoid stalls, while mispredictions flush the pipeline, incurring a performance cost. This experiment uses a vector of random integers to test branching behavior under different conditions:
- **Unpredictable**: `zen::random_int(0, size) > numbers[j]` varies randomly, challenging the branch predictor.
- **Predictable**: `size/2 > numbers[j]` is consistent per element, allowing the predictor to adapt.

### Key Findings

1. **Unpredictable vs. Predictable (Simple Case)**:
   - **Unsorted**: Unpredictable (`0.700319s`) is 71.43% slower than Predictable (`0.200068s`). The random condition prevents the BPT from forming reliable predictions.
   - **Sorted**: Unpredictable (`0.520010s`) is 75.12% slower than Predictable (`0.129374s`). Sorting enhances predictability, reducing mispredictions further.

2. **Sorting Impact (Simple Case)**:
   - Predictable Unsorted (`0.200068s`) vs. Sorted (`0.129374s`): Sorting speeds up execution by 35.34%, as the sorted data forms a single transition (e.g., `true` to `false`), which the BPT learns quickly.

3. **Complex Case**:
   - **Unsorted**: Unpredictable Complex (`4.097937s`) is 11.40% slower than Predictable Complex (`3.630773s`).
   - **Sorted**: Unpredictable Complex (`4.184143s`) is 12.49% slower than Predictable Complex (`3.661655s`).
   - The smaller gap (11-12% vs. 71-75%) reflects the dominance of the `complex_process` function, which overshadows branch prediction penalties.

4. **Unsorted vs. Sorted (Complex Case)**:
   - Percent Difference: -0.85%. Sorting has minimal impact here (`3.630773s` vs. `3.661655s`), as computation time dominates over branch prediction benefits.

5. **Sorting with Unpredictable Conditions**:
   - Unpredictable Unsorted (`0.700319s`) vs. Sorted (`0.520010s`): Sorting reduces time by ~25.74%, likely due to cache locality rather than branch prediction, as the condition remains random.

### Takeaways
- **Predictability Boosts Performance**: Predictable conditions significantly outperform unpredictable ones, especially in simple cases (71-75% faster).
- **Sorting Helps Simple Cases**: Sorting improves predictable simple cases by 35.34%, but its benefit diminishes in complex cases (-0.85%).
- **Complex Workloads Dilute Branch Effects**: In complex scenarios, computation time reduces the relative impact of branch mispredictions (11-12% difference).
- **Small-Scale Caveat**: With small `--size` (e.g., 100) or `--iter` (e.g., 10), trends may not hold due to measurement noise, insufficient BPT warm-up, or cache dominance.

### Why Small Values Differ
Small vector sizes or iteration counts can obscure trends due to:
- **Noise**: Short runtimes (e.g., microseconds) are sensitive to system interference.
- **BPT Warm-Up**: Few iterations limit the predictor’s ability to learn patterns.
- **Cache Effects**: Small data fits in L1 cache, reducing the impact of branch penalties.

For robust results, use larger values like `--size 15000 --iter 5000`, as shown in the example output.

---
