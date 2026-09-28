#include <iostream>
#include <iomanip>
#include <vector>
#include <cmath>
#include <chrono>
#include <algorithm>
#include <thread>
#include <ppl.h>

double f(double x) {
    const int M = std::max(20, static_cast<int>(20.0 * std::fabs(x)));
    const double x2 = x * x;

    std::vector<double> s(M + 1), c(M + 1), cube(M + 1);
    for (int i = 1; i <= M; ++i) {
        s[i] = std::sin(i * x);
        c[i] = std::cos(i * x);
        cube[i] = static_cast<double>(i) * i * i;
    }

    double sum = 0.0;
    for (int k = 1; k <= M; ++k) {
        double inner = 0.0;
        for (int j = 1; j <= M; ++j)
            inner += (k - j) / (x2 + cube[k] + cube[j]) * c[j];
        sum += inner * s[k];
    }
    return (x2 + x) * sum;
}

int main() {
    const int N = 2000;
    std::vector<double> ySeq(N), yPar(N);

    auto t0 = std::chrono::steady_clock::now();
    for (int n = 1; n <= N; ++n)
        ySeq[n - 1] = f(100.0 * std::cos(n));
    auto t1 = std::chrono::steady_clock::now();

    concurrency::parallel_for(1, N + 1, [&](int n) {
        yPar[n - 1] = f(100.0 * std::cos(n));
    });
    auto t2 = std::chrono::steady_clock::now();

    const double seq = std::chrono::duration<double>(t1 - t0).count();
    const double par = std::chrono::duration<double>(t2 - t1).count();

    double maxDiff = 0.0;
    for (int i = 0; i < N; ++i)
        maxDiff = std::max(maxDiff, std::fabs(ySeq[i] - yPar[i]));

    std::cout << std::fixed << std::setprecision(4);
    std::cout << "## Lab 1, part I, variant 7 (parallel_for)\n\n";
    std::cout << "N = " << N << ", threads = " << std::thread::hardware_concurrency() << "\n\n";
    std::cout << "| Version | Time, s |\n|---|---|\n";
    std::cout << "| Sequential for | " << seq << " |\n";
    std::cout << "| concurrency::parallel_for | " << par << " |\n\n";
    std::cout << "Speedup: **" << std::setprecision(2) << seq / par << "x**\n\n";
    std::cout << "Max |y_seq - y_par| = " << std::scientific << maxDiff << "\n\n";

    std::cout << std::fixed << std::setprecision(6);
    std::cout << "First values:\n\n| n | x_n | y_n |\n|---|---|---|\n";
    for (int n = 1; n <= 5; ++n)
        std::cout << "| " << n << " | " << 100.0 * std::cos(n) << " | " << ySeq[n - 1] << " |\n";
    return 0;
}
