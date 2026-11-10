_Run: 2026-11-10 12:49 UTC, CPU: INTEL(R) XEON(R) PLATINUM 8573C, .NET 10.0.403 Release_

## Task I (C#), variant 7: for vs Parallel.For

N = 200 (n = 0..200), processors = 4, time = median of 5 runs

| Version | Time, s |
|---|---|
| for | 0.4601 |
| Parallel.For | 0.2362 |

Speedup: **1.95x**

Max |Y_seq - Y_par| = 0.0E+000

First values:

| n | x_n | y_n |
|---|---|---|
| 0 | 100.000000 | -6.667187 |
| 1 | 54.030231 | -0.045749 |
| 2 | -41.614684 | 0.058113 |
| 3 | -98.999250 | 0.267477 |
| 4 | -65.364362 | -0.043175 |

