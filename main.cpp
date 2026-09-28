#include <iostream>
#include <vector>
#include <cmath>
#include <chrono>
#include <algorithm>
#include <ppl.h>

double f(double x) {
    const int M = std::max(20, (int)(20 * std::fabs(x)));   // max(20, [20|x|])
    const double x2 = x * x;
    double s = 0.0;
    for (int k = 1; k <= M; ++k)
        for (int j = 1; j <= M; ++j)
            s += x2 / (x2 + k * k + j * j) * std::cos((k + j) * x);
    return s;
}

int main() {
    const int N = 2000;
    std::vector<double> y1(N), y2(N);

    // Последовательная версия
    auto t0 = std::chrono::steady_clock::now();
    for (int n = 1; n <= N; ++n)
        y1[n - 1] = f(100 * std::cos(n));
    auto t1 = std::chrono::steady_clock::now();

    // Параллельная версия
    concurrency::parallel_for(1, N + 1, [&](int n) {
        y2[n - 1] = f(100 * std::cos(n));
    });
    auto t2 = std::chrono::steady_clock::now();

    std::chrono::duration<double> seq = t1 - t0, par = t2 - t1;
    std::cout << "Sequential: " << seq.count() << " s\n"
              << "Parallel:   " << par.count() << " s\n"
              << "Speedup:    " << seq.count() / par.count() << "\n";

    // Проверка, что результаты совпадают
    double maxDiff = 0;
    for (int i = 0; i < N; ++i) maxDiff = std::max(maxDiff, std::fabs(y1[i] - y2[i]));
    std::cout << "Max diff:   " << maxDiff << "\n";
}
