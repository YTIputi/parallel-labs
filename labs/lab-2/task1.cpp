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
void runTest(const std::string& name, Container& firstY) {
    Container x;
    for (int n = 1; n <= N; ++n) x.push_back(100.0 * std::cos(n));
    Container ySeq(x.size()), yPar(x.size());

    auto t0 = std::chrono::steady_clock::now();
    std::transform(x.begin(), x.end(), ySeq.begin(), f);
    auto t1 = std::chrono::steady_clock::now();
    concurrency::parallel_transform(x.begin(), x.end(), yPar.begin(), f);
    auto t2 = std::chrono::steady_clock::now();

    const double seq = std::chrono::duration<double>(t1 - t0).count();
    const double par = std::chrono::duration<double>(t2 - t1).count();

    double maxDiff = 0.0;
    auto ia = ySeq.begin();
    for (auto ib = yPar.begin(); ib != yPar.end(); ++ia, ++ib)
        maxDiff = std::max(maxDiff, std::fabs(*ia - *ib));

    std::cout << std::fixed << std::setprecision(4)
              << "| " << name << " | " << seq << " | " << par << " | "
              << std::setprecision(2) << seq / par << "x | "
              << std::scientific << std::setprecision(1) << maxDiff << " |\n";

    if (firstY.empty()) firstY = ySeq;
}

int main() {
    std::cout << "## Lab 2, variant 7 (std::transform vs parallel_transform)\n\n";
    std::cout << "N = " << N << ", threads = " << std::thread::hardware_concurrency() << "\n\n";
    std::cout << "| Container | std::transform, s | concurrency::parallel_transform, s | Speedup | Max diff |\n";
    std::cout << "|---|---|---|---|---|\n";

    std::vector<double> y;
    runTest<std::vector<double>>("vector", y);
    std::deque<double> yd;
    runTest<std::deque<double>>("deque", yd);
    std::list<double> yl;
    runTest<std::list<double>>("list", yl);

    std::cout << "\nFirst values:\n\n| n | x_n | y_n |\n|---|---|---|\n" << std::fixed;
    for (int n = 1; n <= 5; ++n)
        std::cout << "| " << n << " | " << std::setprecision(6) << 100.0 * std::cos(n)
                  << " | " << y[n - 1] << " |\n";
    return 0;
}
