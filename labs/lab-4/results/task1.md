_Run: 2026-10-05 17:03 UTC, CPU: AMD EPYC 7763 64-Core Processor                , MSVC Release /O2_

## Lab 4, variant 6 (sorting std::vector<unsigned int>)

N = 50000000, threads = 4, best of 3 runs

| Algorithm | Time, s | Speedup vs std::sort | Result matches std::sort |
|---|---|---|---|
| std::sort | 4.6702 | 1.00x | - |
| concurrency::parallel_sort | 1.5244 | 3.06x | yes |
| concurrency::parallel_buffered_sort | 1.4406 | 3.24x | yes |
| concurrency::parallel_radixsort | 0.3562 | 13.11x | yes |

First x_n: 0, 1682941969, 2909751765, 508032029, 2849138805
Sorted: min = 0, median = 2828423891, max = 3999999999

