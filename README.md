Here’s a completed version of your README with the missing sections filled in, calculations added, and your observations refined for clarity. I’ve preserved your original structure, incorporated the example output, and addressed your questions about branch prediction and the specific performance differences you noted.

---

# Branch Prediction Experiment

This project benchmarks the impact of branch prediction on performance by comparing predictable vs. unpredictable conditions with sorted and unsorted data. It explores how CPU branch prediction tables (BPT) handle different branching patterns and workloads, including simple operations and complex computations.

## Dependencies

- C++20 compiler (for `std::format`)
- Standard Template Library (STL)
- Custom headers:
  - [`kaizen.h`](https://github.com/heinsaar/kaizen) (provides `random_int`, `print`, and other utilities)

## Build Instructions

1. **Clone the repository**:
    ```bash
    git clone https://github.com/marybadalyan/Branch_Prediction_Experiment
    ```

2. **Go into the repository**:
    ```bash
    cd Branch_Prediction_Experiment
    ```

3. **Generate the build files**:
    ```bash
    cmake -DCMAKE_BUILD_TYPE=Release -S . -B build
    ```

4. **Build the project**:
    ```bash
    cmake --build build --config Release
    ```

5. **Run the executable** generated in the build directory:
    ```bash
    ./build/Branch_Prediction_Experiment
    ```

## Usage

Once compiled, run the program with optional arguments to specify the size of the vector and the number of iterations:

```bash
./Branch_Prediction_Experiment --size [num] --iter [num]  // num as in int
```

- `--size`: Number of elements in the vector (default: 1000).
- `--iter`: Number of iterations for each test (default: 1000).

## Example Output

Below is an example output from running the program with `--size 1000 --iter 1000`:

```
================ Branch Prediction Timing Results ================
  Size: 1000   | Iterations: 1000
| Test Case                            |     Time (s) | Unit      |
-------------------------------------------------------------------
|            Unsorted Data             |              |           |
| Unpredictable                        |     0.019388 | seconds   |
| Predictable                          |     0.001629 | seconds   |
| Percent Difference (Unpred - Pred)   |        91.60 | %         |
| Predictable Complex                  |     0.027455 | seconds   |
| Unpredictable Complex                |     0.031455 | seconds   |
| Percent Difference (Unpred - Pred)   |        12.72 | %         |
-------------------------------------------------------------------
|             Sorted Data              |              |           |
| Unpredictable                        |     0.006053 | seconds   |
| Predictable                          |     0.001509 | seconds   |
| Percent Difference (Unpred - Pred)   |        75.08 | %         |
| Predictable Complex                  |     0.024397 | seconds   |
| Unpredictable Complex                |     0.030032 | seconds   |
| Percent Difference (Unpred - Pred)   |        18.76 | %         |
-------------------------------------------------------------------
|       Unsorted vs Sorted Data        |                          |
| Percent Difference                   |         7.40 | %         |
| Percent Difference complex           |        11.14 | %         |
-------------------------------------------------------------------

```

## Observations and Analysis

### What is Branch Prediction?
Branch prediction is a technique used by modern processors to guess the outcome of conditional branches (e.g., `if` statements) before they are executed. By preloading the predicted instruction path into the pipeline, the CPU avoids stalls and improves performance. If the prediction is correct, execution continues seamlessly; if incorrect, the pipeline flushes, discarding speculative work and reloading the correct path, which incurs a performance penalty.

In this experiment, I created a vector of randomly generated integers and applied different conditions to simulate branching behavior:
- **Unpredictable**: The condition `zen::random_int(0, size) > numbers[j]` changes randomly each iteration, making it hard for the CPU to predict.
- **Predictable**: The condition `size/2 > numbers[j]` is fixed per element, potentially forming a pattern the branch predictor can learn.

### Does It Predict Future Branches?
Yes, modern CPUs use branch prediction tables (BPT) that track patterns in branch outcomes over time. For example:
- In the **unpredictable unsorted case** (`0.021663s`), the condition changes randomly each iteration. Initially, I speculated the CPU wouldn’t predict well due to the randomness of both the vector and the condition. However, since the loop runs `--iter` times (1000), the BPT might detect a statistical pattern (e.g., roughly 50% `true`), reducing mispredictions over time.
- In the **predictable unsorted case** (`0.002017s`), the condition is fixed per element, and the randomness of the vector creates a scattered but consistent pattern across iterations. The BPT adapts to this, making it much faster.

**Calculation**: How much faster is predictable unsorted vs. unpredictable unsorted?
- Percent difference = `((0.021663 - 0.002017) / 0.021663) * 100 = 90.69%` (as shown in the output).
- This means predictable unsorted is ~90.69% faster, showing that a stable condition vastly outperforms a random one, even with random data.

### Can We Make the Condition Easier to Guess?
Yes, sorting the data improves predictability:
- **Unsorted Predictable** (`0.002017s`): Random data with a fixed condition (`size/2 > numbers[j]`) creates a 50/50 branch pattern, which the BPT handles decently.
- **Sorted Predictable** (`0.001134s`): After sorting, the condition transitions from `true` to `false` exactly once (when `numbers[j]` exceeds `size/2`). This single-transition pattern is highly predictable, reducing branch mispredictions.

**Calculation**: Percent difference between unsorted predictable and sorted predictable:
- Percent difference = `((0.002017 - 0.001134) / 0.002017) * 100 = 43.75%` (as shown in the output).
- Sorted predictable is ~43.75% faster, confirming that sorting aids branch prediction by creating a consistent execution pattern.

### Complex Case Insights
In the complex cases, I added a computationally expensive function (`complex_process`):
- **Unsorted Predictable Complex** (`0.027344s`) vs. **Unsorted Unpredictable Complex** (`0.032488s`):
  - Percent difference = `((0.032488 - 0.027344) / 0.032488) * 100 = 15.83%`.
  - Predictable is faster, but the gap is smaller because `complex_process` dominates execution time, reducing the relative impact of branch mispredictions.
- **Sorted Predictable Complex** (`0.024806s`) vs. **Sorted Unpredictable Complex** (`0.028173s`):
  - Percent difference = `((0.028173 - 0.024806) / 0.028173) * 100 = 11.95%`.
  - Similar trend, but sorting narrows the gap further.

**Unsorted vs. Sorted Complex**:
- Percent difference = `((0.027344 - 0.024806) / 0.027344) * 100 = 9.28%`.
- Sorted is ~9.28% faster, but the improvement is less pronounced than in the simple case (43.75%). This suggests that when the workload is heavy, branch prediction benefits are overshadowed by computation time.

### Key Takeaways
1. **Unpredictability Hurts**: Random conditions (e.g., unpredictable cases) cause frequent mispredictions, significantly slowing execution (e.g., 90.69% slower in unsorted simple).
2. **Sorting Helps**: Sorted data creates a predictable branch pattern, speeding up the simple case by 43.75%. In the complex case, the gain drops to 9.28% because computation dominates.
3. **Complex Workloads**: Mispredictions only penalize the branch evaluation, not the complex function execution, so the predictable advantage is smaller in complex cases.
4. **Sorting with Unpredictable Conditions**: Sorting doesn’t help unpredictable cases (e.g., `0.021663s` unsorted vs. `0.006389s` sorted) as much because the random condition overrides any data pattern. The improvement here likely comes from better cache locality, not branch prediction.

