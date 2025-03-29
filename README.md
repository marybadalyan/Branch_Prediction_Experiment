

## Dependencies

- C++20 compiler (for std::format)
- Standard Template Library (STL)
- Custom headers:
  - [`kaizen.h`](https://github.com/heinsaar/kaizen) (random_int utilities)
 
  
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
Once compiled, run the program to start the memory stress test:

```bash
./Branch_Prediction_Experiment --size [num] --iter [num]  // num as in int 
```


## Dependencies

- C++20 compiler (for std::format)
- Standard Template Library (STL)
- Custom headers:
  - [`kaizen.h`](https://github.com/heinsaar/kaizen) (timer, print, random_int utilities)
  - `Rec_MatMul.h` (matrix multiplication implementations)
  - `cache_size.h` (cache-related constants)


## Example output 
```

================ Branch Prediction Timing Results ================
  Size: 1000   | Iterations: 1000
| Test Case                            |     Time (s) | Unit      |
-------------------------------------------------------------------
|            Unsorted Data             |              |           |
| Unpredictable                        |     0.018527 | seconds   |
| Predictable                          |     0.001608 | seconds   |
| Percent Difference (Unpred - Pred)   |        91.32 | %         |
| Predictable Complex                  |     0.024241 | seconds   |
| Unpredictable Complex                |     0.029932 | seconds   |
| Percent Difference (Unpred - Pred)   |        19.01 | %         |
-------------------------------------------------------------------
|             Sorted Data              |              |           |
| Unpredictable                        |     0.006540 | seconds   |
| Predictable                          |     0.001069 | seconds   |
| Percent Difference (Unpred - Pred)   |        83.66 | %         |
| Predictable Complex                  |     0.021965 | seconds   |
| Unpredictable Complex                |     0.027360 | seconds   |
| Percent Difference (Unpred - Pred)   |        19.72 | %         |
-------------------------------------------------------------------
|       Unsorted vs Sorted Data        |                          |
| Percent Difference                   |        33.53 | %         |
| Percent Difference complex           |         9.39 | %         |
-------------------------------------------------------------------
PS C:\Users\MeriBadalyan\Branch_Prediction_Experiment> cmake --build build --config Release
MSBuild version 17.13.9+e0f243f1e for .NET Framework

  Branch_Prediction_Experiment.vcxproj -> C:\Users\MeriBadalyan\Branch_Prediction_Experiment\build\Release\Branch_Prediction_Experiment.exe
PS C:\Users\MeriBadalyan\Branch_Prediction_Experiment> ./build/Release/Branch_Prediction_Experiment.exe --size 1000 --iter 1000

================ Branch Prediction Timing Results ================
  Size: 1000   | Iterations: 1000
| Test Case                            |     Time (s) | Unit      |
-------------------------------------------------------------------
|            Unsorted Data             |              |           |
| Unpredictable                        |     0.021663 | seconds   |
| Predictable                          |     0.002017 | seconds   |
| Percent Difference (Unpred - Pred)   |        90.69 | %         |
| Predictable Complex                  |     0.027344 | seconds   |
| Unpredictable Complex                |     0.032488 | seconds   |
| Percent Difference (Unpred - Pred)   |        15.83 | %         |
-------------------------------------------------------------------
|             Sorted Data              |              |           |
| Unpredictable                        |     0.006389 | seconds   |
| Predictable                          |     0.001134 | seconds   |
| Percent Difference (Unpred - Pred)   |        82.25 | %         |
| Predictable Complex                  |     0.024806 | seconds   |
| Unpredictable Complex                |     0.028173 | seconds   |
| Percent Difference (Unpred - Pred)   |        11.95 | %         |
-------------------------------------------------------------------
|       Unsorted vs Sorted Data        |                          |
| Percent Difference                   |        43.75 | %         |
| Percent Difference complex           |         9.28 | %         |
-------------------------------------------------------------------
```

դիտարկած երեվությներ  does it predict few future branches ? 

I made a vector with random genereated numbers that for some  condition  to do some work
what  we see here  is a common example of Branch prediction
Branch prediction is a processor technique to guess conditional branch outcomes, preloading instructions to boost speed. It relies on patterns or hints. If correct, execution flows smoothly; if wrong, it stalls the processor due to pipeline flushing, discarding incorrect work and reloading the right path.
can we make the condition easier to guess ?
take a look at  Unpredictable (unsorted)  where the if(condition) is constatntly chacnges  as random integer predictapility of the outcome changes vs if i kept a arndom integer array the BPT still geneeraets a pattern the same pattern for --iter times we can see how predictable that is benchmark shows us   0.000027 for  Unpredictable (unsorted) and   Predictable (unsorted)    0.000005    ~ calculate this % faster 
speculation was that the cpu would not be able to guess because the vector itself is randomly generated but CPU BPT predicts future outcomes as a pattern and iterations accure as we can see so the condition unpredictability has great disatvantage 
speculation also suggests that using a complex function and having sudden conditional change will potetentially flush the pipeline causing time stalling and function that is potentially more complex branch prediction would do nothing but  that was wrong your prediction miss only gains the time to evaluate the if not the time to evaluate the complex function so there should be differnce between  unpredictable complex and predictable complex 
with predictable complex being faster  in most cases where size is small due to the probability of executing the if statements 
while sorting here will only aid our predictabillity 
due to the branch execution consisntency 
lets compare how it affetc both of corse sorting with unpredictable condition deoesnt make sense since it wont affect it in real time becouse the conditions ara randomly generated each iteration sorting it wont help prediciton since we dont know what were comparing it with 

as we can see in  Predictable  sorted and   Predictable  unsorted  with over   ~ calculate please% difefrence 
as we can see in  Predictable complex   sorted and   Predictable  complex unsorted  with over   ~ calculate please% difefrence  but since complex function always returnes a different output that is not consistnt wit the previous one we get only 