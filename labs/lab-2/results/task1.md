_Run: 2026-10-05 16:55 UTC, CPU: AMD EPYC 9V45 96-Core Processor                , MSVC Release /O2_

## Lab 2, variant 7 (std::transform vs parallel_transform)

N = 2000, threads = 4

| Container | std::transform, s | concurrency::parallel_transform, s | Speedup | Max diff |
|---|---|---|---|---|
| vector | 3.6905 | 1.8345 | 2.01x | 0.0e+00 |
| deque | 3.6617 | 1.9028 | 1.92x | 0.0e+00 |
| list | 3.6942 | 1.8492 | 2.00x | 0.0e+00 |

First values:

| n | x_n | y_n |
|---|---|---|
| 1 | 54.030231 | -0.045749 |
| 2 | -41.614684 | 0.058113 |
| 3 | -98.999250 | 0.267477 |
| 4 | -65.364362 | -0.043175 |
| 5 | 28.366219 | -0.005352 |

