_Run: 2026-10-05 17:10 UTC, CPU: AMD EPYC 7763 64-Core Processor                , MSVC Release /O2_

## Lab 5, variant 7 (concurrency::task_group)

N = 1000, threads = 4

| Version | Time, s |
|---|---|
| Sequential for | 2.9364 |
| task_group, 100 tasks | 1.4730 |

Speedup: **1.99x**

Max |y_seq - y_par| = 0.00e+00

Effect of the number of tasks:

| Tasks | Elements per task | Time, s | Speedup |
|---|---|---|---|
| 1 | 1000 | 2.9250 | 1.00x |
| 4 | 250 | 1.4643 | 2.01x |
| 16 | 62 | 1.5286 | 1.92x |
| 100 | 10 | 1.4602 | 2.01x |
| 1000 | 1 | 1.4616 | 2.01x |

First values:

| n | x_n | y_n |
|---|---|---|
| 1 | 54.030231 | -0.045749 |
| 2 | -41.614684 | 0.058113 |
| 3 | -98.999250 | 0.267477 |
| 4 | -65.364362 | -0.043175 |
| 5 | 28.366219 | -0.005352 |

