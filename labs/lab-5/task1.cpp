// Lab 5, variant 7: parallelization with concurrency::task_group
// x_n = 100 cos n, n = 1..N, y_n = f(x_n), N = 1000
// f(x) = sum_{k=1}^{M} sum_{j=1}^{M} (x^2 + x)(k - j) / (x^2 + k^3 + j^3) * sin(kx) cos(jx),
// M = max(20, [20|x|])
#include <iostream>
#include <iomanip>
#include <vector>
#include <string>
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

const int N = 1000;

void parallelVersion(const std::vector<double>& x, std::vector<double>& y, int tasks) {
    concurrency::task_group tg;
    const int n = static_cast<int>(x.size());
    for (int t = 0; t < tasks; ++t) {
        const int begin = static_cast<int>(static_cast<long long>(n) * t / tasks);
        const int end = static_cast<int>(static_cast<long long>(n) * (t + 1) / tasks);
        tg.run([&x, &y, begin, end] {
            for (int i = begin; i < end; ++i) y[i] = f(x[i]);
        });
    }
    tg.wait();
}

double maxDiff(const std::vector<double>& a, const std::vector<double>& b) {
    double d = 0.0;
    for (size_t i = 0; i < a.size(); ++i) d = std::max(d, std::fabs(a[i] - b[i]));
    return d;
}

int main() {
    std::vector<double> x(N), ySeq(N), yPar(N);
    for (int n = 1; n <= N; ++n) x[n - 1] = 100.0 * std::cos(n);

    auto t0 = std::chrono::steady_clock::now();
    for (int i = 0; i < N; ++i) ySeq[i] = f(x[i]);
    auto t1 = std::chrono::steady_clock::now();
    const double seq = std::chrono::duration<double>(t1 - t0).count();

    const int threads = static_cast<int>(std::max(1u, std::thread::hardware_concurrency()));
    const int MAIN_TASKS = 100;

    t0 = std::chrono::steady_clock::now();
    parallelVersion(x, yPar, MAIN_TASKS);
    t1 = std::chrono::steady_clock::now();
    const double par = std::chrono::duration<double>(t1 - t0).count();
    const double diff = maxDiff(ySeq, yPar);

    std::cout << "## Lab 5, variant 7 (concurrency::task_group)\n\n";
    std::cout << "N = " << N << ", threads = " << threads << "\n\n";
    std::cout << "| Version | Time, s |\n|---|---|\n" << std::fixed << std::setprecision(4);
    std::cout << "| Sequential for | " << seq << " |\n";
    std::cout << "| task_group, " << MAIN_TASKS << " tasks | " << par << " |\n\n";
    std::cout << "Speedup: **" << std::setprecision(2) << seq / par << "x**\n\n";
    std::cout << "Max |y_seq - y_par| = " << std::scientific << std::setprecision(2) << diff << "\n\n";

    std::cout << "Effect of the number of tasks:\n\n";
    std::cout << "| Tasks | Elements per task | Time, s | Speedup |\n|---|---|---|---|\n";
    const int counts[] = {1, threads, 4 * threads, 100, 1000};
    bool allOk = diff == 0.0;
    for (int tasks : counts) {
        t0 = std::chrono::steady_clock::now();
        parallelVersion(x, yPar, tasks);
        t1 = std::chrono::steady_clock::now();
        const double t = std::chrono::duration<double>(t1 - t0).count();
        if (maxDiff(ySeq, yPar) != 0.0) allOk = false;
        std::cout << std::fixed << "| " << tasks << " | " << N / tasks << " | "
                  << std::setprecision(4) << t << " | " << std::setprecision(2) << seq / t << "x |\n";
    }

    std::cout << "\nFirst values:\n\n| n | x_n | y_n |\n|---|---|---|\n" << std::fixed;
    for (int n = 1; n <= 5; ++n)
        std::cout << "| " << n << " | " << std::setprecision(6) << x[n - 1]
                  << " | " << ySeq[n - 1] << " |\n";
    return allOk ? 0 : 1;
}
