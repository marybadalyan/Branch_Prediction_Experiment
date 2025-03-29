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

Below is an example output from running the program with `--size 15000 --iter 5000`:

```

 ================ Branch Prediction Timing Results ================
  Size: 15000  | Iterations: 5000
| Test Case                            |     Time (s) | Unit      |
-------------------------------------------------------------------
|            Unsorted Data             |              |           |
| Unpredictable                        |     0.642121 | seconds   |
| Predictable                          |     0.189645 | seconds   |
| Percent Difference (Unpred - Pred)   |        70.47 | %         |
| Predictable Complex                  |     1.986897 | seconds   |
| Unpredictable Complex                |     2.679526 | seconds   |
| Percent Difference (Unpred - Pred)   |        25.85 | %         |
-------------------------------------------------------------------
|             Sorted Data              |              |           |
| Unpredictable                        |     0.547439 | seconds   |
| Predictable                          |     0.089864 | seconds   |
| Percent Difference (Unpred - Pred)   |        83.58 | %         |
| Predictable Complex                  |     1.902097 | seconds   |
| Unpredictable Complex                |     2.474131 | seconds   |
| Percent Difference (Unpred - Pred)   |        23.12 | %         |
-------------------------------------------------------------------
|       Unsorted vs Sorted Data        |                          |
| Percent Difference                   |        52.61 | %         |
| Percent Difference complex           |         4.27 | %         |
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
- In the **unpredictable unsorted case** (`0.642121s`), the condition changes randomly each iteration. Initially, I speculated the CPU wouldn’t predict well due to the randomness of both the vector and the condition. However, since the loop runs `--iter` times (5000), the BPT might detect a statistical pattern (e.g., roughly 50% `true`), reducing mispredictions over time.
- In the **predictable unsorted case** (`0.189645s`), the condition is fixed per element, and the randomness of the vector creates a scattered but consistent pattern across iterations. The BPT adapts to this, making it much faster.

**Calculation**: How much faster is predictable unsorted vs. unpredictable unsorted?
- Percent difference = `((0.642121 - 0.189645) / 0.642121) * 100 = 70.47%` (as shown in the output).
- This means predictable unsorted is ~70.47% faster, showing that a stable condition vastly outperforms a random one, even with random data.

### Can We Make the Condition Easier to Guess?
Yes, sorting the data improves predictability:
- **Unsorted Predictable** (`0.189645s`) vs. **Sorted Predictable** (`0.089864s`):
  - Percent difference = `((0.189645 - 0.089864) / 0.189645) * 100 = 52.61%` (as shown in the output under "Unsorted vs Sorted Data").
  - Sorted predictable is ~52.61% faster, confirming that sorting aids branch prediction by creating a consistent execution pattern (e.g., a single transition from `true` to `false` when `numbers[j]` exceeds `size/2`)).

### Complex Case Insights
In the complex cases, I added a computationally expensive function (`complex_process`):
- **Unsorted Predictable Complex** (`1.986897s`) vs. **Unsorted Unpredictable Complex** (`2.679526s`):
  - Percent difference = `((2.679526 - 1.986897) / 2.679526) * 100 = 25.85%` (as shown in the output).
  - Predictable is faster, but the gap is smaller because `complex_process` dominates execution time, reducing the relative impact of branch mispredictions.
- **Sorted Predictable Complex** (`1.902097s`) vs. **Sorted Unpredictable Complex** (`2.474131s`):
  - Percent difference = `((2.474131 - 1.902097) / 2.474131) * 100 = 23.12%` (as shown in the output).
  - Similar trend, but sorting narrows the gap further.

**Unsorted vs. Sorted Complex**:
- Percent difference = `((1.986897 - 1.902097) / 1.986897) * 100 = 4.27%` (as shown in the output).
- Sorted is ~4.27% faster, but the improvement is less pronounced than in the simple case (52.61%). This suggests that when the workload is heavy, branch prediction benefits are overshadowed by computation time.

### Key Takeaways
1. **Unpredictability Hurts**: Random conditions (e.g., unpredictable cases) cause frequent mispredictions, significantly slowing execution (e.g., 70.47% slower in unsorted simple).
2. **Sorting Helps**: Sorted data creates a predictable branch pattern, speeding up the simple case by 52.61%. In the complex case, the gain drops to 4.27% because computation dominates.
3. **Complex Workloads**: Mispredictions only penalize the branch evaluation, not the complex function execution, so the predictable advantage is smaller in complex cases (25.85% unsorted, 23.12% sorted).
4. **Sorting with Unpredictable Conditions**: Sorting reduces the unpredictable case time from `0.642121s` (unsorted) to `0.547439s` (sorted), a difference of `((0.642121 - 0.547439) / 0.642121) * 100 = 14.74%`. This improvement likely comes from better cache locality, not branch prediction, as the condition remains random.
5. **Note on Small Values**: These trends (e.g., predictable outperforming unpredictable, sorting improving performance) might not always hold with small vector sizes (e.g., `--size 100`) or few iterations (e.g., `--iter 10`). With smaller workloads, execution times can be too short to measure reliably, and the branch predictor may not have enough data to adapt effectively.

---

### Why This Happens
The observation that trends might not hold with small values or iterations stems from several factors related to how CPUs and branch predictors work:

1. **Measurement Noise**: When the vector size (`--size`) or number of iterations (`--iter`) is small, the total execution time becomes very short (e.g., microseconds or less). At such scales, external factors like system interrupts, timer resolution, or compiler optimizations (e.g., loop unrolling) can introduce noise that overshadows the actual performance differences between predictable and unpredictable cases. For example, if both cases take 0.0001s, a tiny fluctuation could flip the results.

2. **Branch Predictor Warm-Up**: Modern branch predictors rely on historical data stored in the branch prediction table (BPT) to learn patterns. With few iterations (e.g., `--iter 10`), the predictor might not have enough repetitions to identify a pattern, especially in the predictable case. For instance, the sorted predictable case benefits from a single transition (all `true` to all `false`), but with only 10 iterations, the BPT might not "warm up" enough to capitalize on this, reducing the expected speedup.

3. **Cache Effects Dominate**: With small vector sizes (e.g., `--size 100`), the entire dataset fits easily into the CPU’s L1 cache (typically 32-64 KB). This minimizes memory access latency, making branch prediction penalties less significant compared to cache effects. In the example output (`--size 15000`), the data is larger (15000 integers × 4 bytes = 60 KB), often exceeding L1 cache, so branch mispredictions have a more noticeable impact alongside cache misses.

4. **Statistical Insignificance**: In the unpredictable case, the condition relies on randomness (`zen::random_int`). With few iterations or a small vector, the random outcomes might accidentally skew toward a predictable pattern (e.g., mostly `true`), making it appear faster than expected. With larger sizes and iterations (e.g., 15000 and 5000), the randomness averages out, exposing the true cost of mispredictions.

5. **Overhead of Sorting**: Sorting the data (which improves predictability) has a computational cost (e.g., O(n log n)). For small `--size`, this overhead might outweigh the branch prediction benefits, especially if the subsequent loop runs few times, negating the advantage seen in the larger dataset (52.61% speedup).

In summary, small values or iterations reduce the signal-to-noise ratio, limit the branch predictor’s learning opportunity, and amplify unrelated factors like cache or sorting overhead, making the trends less consistent or even reversed compared to the robust results seen with `--size 15000 --iter 5000`.