_Run: 2026-10-05 17:02 UTC, CPU: AMD EPYC 9V45 96-Core Processor                , MSVC Release /O2_

## Lab 3, variant 6 (sorting std::vector<double>)

N = 50000000, threads = 4, best of 3 runs

| Algorithm | Time, s | Speedup vs std::sort | Result matches std::sort |
|---|---|---|---|
| std::sort | 4.6826 | 1.00x | - |
| concurrency::parallel_sort | 1.4195 | 3.30x | yes |
| concurrency::parallel_buffered_sort | 1.3145 | 3.56x | yes |

First x_n: 0.000000, 0.420735, 0.727438, 0.127008, -0.712285
Sorted: min = -1.000000, median = -0.000000, max = 1.000000

