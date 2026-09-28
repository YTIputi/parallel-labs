_Run: 2026-09-28 14:00 UTC, CPU: AMD EPYC 7763 64-Core Processor                , MSVC Release /O2_

## Lab 1, part I, variant 7 (parallel_for)

N = 2000, threads = 4

| Version | Time, s |
|---|---|
| Sequential for | 5.9258 |
| concurrency::parallel_for | 2.9237 |

Speedup: **2.03x**

Max |y_seq - y_par| = 0.00e+00

First values:

| n | x_n | y_n |
|---|---|---|
| 1 | 54.030231 | -0.045749 |
| 2 | -41.614684 | 0.058113 |
| 3 | -98.999250 | 0.267477 |
| 4 | -65.364362 | -0.043175 |
| 5 | 28.366219 | -0.005352 |

