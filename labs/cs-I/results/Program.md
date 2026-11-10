_Run: 2026-11-10 11:58 UTC, CPU: AMD EPYC 9V45 96-Core Processor                , .NET 10.0.403 Release_

## Task I (C#), variant 7: for vs Parallel.For

N = 200 (n = 0..200), processors = 4

| Version | Time, s |
|---|---|
| for | 0.4107 |
| Parallel.For | 0.4138 |

Speedup: **0.99x**

Max |Y_seq - Y_par| = 0.0E+000

First values:

| n | x_n | y_n |
|---|---|---|
| 0 | 100.000000 | -6.667187 |
| 1 | 54.030231 | -0.045749 |
| 2 | -41.614684 | 0.058113 |
| 3 | -98.999250 | 0.267477 |
| 4 | -65.364362 | -0.043175 |

