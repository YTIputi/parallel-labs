_Run: 2026-10-13 12:11 UTC, CPU: AMD EPYC 9V74 80-Core Processor                , MSVC Release /O2_

## Task 7, variant 18 (pipeline f = f2 o f1)

Grid 800 x 800 on [-5, 5]^2, K = 500 factors (250 per stage), threads = 4

| Version | Time, s | Speedup | Max diff |
|---|---|---|---|
| Sequential | 1.0949 | 1.00x | - |
| a) unbounded_buffer + agents | 0.7341 | 1.49x | 0.0e+00 |
| b) transformer | 0.6486 | 1.69x | 0.0e+00 |

Sample values:

| x | y | f(x,y) |
|---|---|---|
| -5.000000 | -5.000000 | -0.002174 |
| -2.496871 | 0.006258 | 0.004294 |
| 0.006258 | -1.670839 | 0.923919 |
| 2.509387 | 5.000000 | -0.051658 |
| 5.000000 | -2.997497 | 0.000942 |

