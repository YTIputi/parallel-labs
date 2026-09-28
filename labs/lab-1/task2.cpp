#include <iostream>
#include <iomanip>
#include <vector>
#include <deque>
#include <list>
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

const int N = 2000;

template <class Container>
void runTest(const std::string& name) {
    Container a, b;
    for (int n = 1; n <= N; ++n) {
        a.push_back(100.0 * std::cos(n));
        b.push_back(100.0 * std::cos(n));
    }

    auto t0 = std::chrono::steady_clock::now();
    std::for_each(a.begin(), a.end(), [](double& x) { x = f(x); });
    auto t1 = std::chrono::steady_clock::now();
    concurrency::parallel_for_each(b.begin(), b.end(), [](double& x) { x = f(x); });
    auto t2 = std::chrono::steady_clock::now();

    const double seq = std::chrono::duration<double>(t1 - t0).count();
    const double par = std::chrono::duration<double>(t2 - t1).count();

    double maxDiff = 0.0;
    auto ia = a.begin();
    for (auto ib = b.begin(); ib != b.end(); ++ia, ++ib)
        maxDiff = std::max(maxDiff, std::fabs(*ia - *ib));

    std::cout << std::fixed << std::setprecision(4)
              << "| " << name << " | " << seq << " | " << par << " | "
              << std::setprecision(2) << seq / par << "x | "
              << std::scientific << std::setprecision(1) << maxDiff << " |\n";
}

int main() {
    std::cout << "## Lab 1, part II (f from variant 7)\n\n";
    std::cout << "N = " << N << ", threads = " << std::thread::hardware_concurrency() << "\n\n";
    std::cout << "| Container | std::for_each, s | concurrency::parallel_for_each, s | Speedup | Max diff |\n";
    std::cout << "|---|---|---|---|---|\n";

    runTest<std::vector<double>>("vector");
    runTest<std::deque<double>>("deque");
    runTest<std::list<double>>("list");
    return 0;
}
