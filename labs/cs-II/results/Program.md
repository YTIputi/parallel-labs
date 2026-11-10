_Run: 2026-11-10 12:41 UTC, CPU: Intel(R) Xeon(R) 6973P-C, .NET 10.0.403 Release_

## Task II (C#), variant 7: foreach vs Parallel.ForEach (array)

N = 200 (n = 0..200), processors = 4

| Version | S | Time, s | Speedup | Diff |
|---|---|---|---|---|
| foreach | -27.8300522504 | 0.4742 | 1.00x | - |
| Parallel.ForEach + lock | -27.8300522504 | 0.3568 | 1.33x | 5.4E-012 |
| Parallel.ForEach + localInit/localFinally | -27.8300522504 | 0.2316 | 2.05x | 1.1E-012 |

