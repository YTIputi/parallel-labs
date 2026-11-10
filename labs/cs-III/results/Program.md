_Run: 2026-11-10 12:42 UTC, CPU: INTEL(R) XEON(R) PLATINUM 8573C, .NET 10.0.403 Release_

## Task III (C#), variant 7: foreach vs Parallel.ForEach (List / Stack / Queue)

N = 200 (n = 0..200), processors = 4

| Container | S foreach | S Parallel.ForEach | T foreach, s | T Parallel.ForEach, s | Speedup | Diff |
|---|---|---|---|---|---|---|
| List | -27.8300522504 | -27.8300522504 | 0.5153 | 0.5069 | 1.02x | 2.0E-012 |
| Stack | -27.8300522504 | -27.8300522504 | 0.4773 | 0.2504 | 1.91x | 3.5E-012 |
| Queue | -27.8300522504 | -27.8300522504 | 0.4803 | 0.2467 | 1.95x | 4.5E-012 |

