_Run: 2026-10-13 12:09 UTC, CPU: AMD EPYC 7763 64-Core Processor                , MSVC Release /O2_

## Task 6, variant 6 (concurrent_vector / concurrent_queue)

Grid 400 x 400 = 160000 points, N2 = 50, threads = 4

Kept points (df/dy <= 0): 73016 of 160000 (45.6%)

| Version | Container | Time, s | Speedup | Same points | Max diff f |
|---|---|---|---|---|---|
| Sequential | std::vector | 0.4667 | 1.00x | - | - |
| parallel_for | concurrent_vector | 0.1921 | 2.43x | yes | 0.0e+00 |
| parallel_for | concurrent_queue | 0.2632 | 1.77x | yes | 0.0e+00 |

First kept points:

| x | y | f(x,y) |
|---|---|---|
| 0.000000 | 0.598399 | 0.367019 |
| 0.000000 | 0.614146 | 0.366915 |
| 0.000000 | 0.661388 | 0.366975 |
| 0.000000 | 0.677135 | 0.366786 |
| 0.000000 | 0.692883 | 0.366269 |

