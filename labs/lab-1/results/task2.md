_Run: 2026-09-28 14:00 UTC, CPU: AMD EPYC 7763 64-Core Processor                , MSVC Release /O2_

## Lab 1, part II (f from variant 7)

N = 2000, threads = 4

| Container | std::for_each, s | concurrency::parallel_for_each, s | Speedup | Max diff |
|---|---|---|---|---|
| vector | 5.8349 | 2.9028 | 2.01x | 0.0e+00 |
| deque | 5.8481 | 2.9042 | 2.01x | 0.0e+00 |
| list | 5.8458 | 2.8997 | 2.02x | 0.0e+00 |

