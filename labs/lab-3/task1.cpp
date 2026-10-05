// Lab 3, variant 6: sorting doubles
// x_n = n^2 / (1 + n^2) * sin(n), n = 0..N-1, N = 50 000 000
// std::sort vs concurrency::parallel_sort vs concurrency::parallel_buffered_sort, std::vector
#include <iostream>
#include <iomanip>
#include <vector>
#include <string>
#include <cmath>
#include <chrono>
#include <algorithm>
#include <functional>
#include <thread>
#include <ppl.h>

const int N = 50000000;
const int RUNS = 3;  // each sort is timed 3 times, the best time is reported

// Times sortFn on a fresh copy of src, returns the best of RUNS; sorted result stays in out
double timeSort(const std::vector<double>& src, std::vector<double>& out,
                const std::function<void(std::vector<double>&)>& sortFn) {
    double best = 1e100;
    for (int r = 0; r < RUNS; ++r) {
        out = src;
        auto t0 = std::chrono::steady_clock::now();
        sortFn(out);
        auto t1 = std::chrono::steady_clock::now();
        best = std::min(best, std::chrono::duration<double>(t1 - t0).count());
    }
    return best;
}

int main() {
    std::vector<double> x(N);
    for (int n = 0; n < N; ++n) {
        const double n2 = static_cast<double>(n) * n;
        x[n] = n2 / (1.0 + n2) * std::sin(static_cast<double>(n));
    }

    std::vector<double> ref, work;
    const double tStd = timeSort(x, ref, [](std::vector<double>& v) {
        std::sort(v.begin(), v.end());
    });
    const double tPar = timeSort(x, work, [](std::vector<double>& v) {
        concurrency::parallel_sort(v.begin(), v.end());
    });
    const bool okPar = std::is_sorted(work.begin(), work.end()) && work == ref;
    const double tBuf = timeSort(x, work, [](std::vector<double>& v) {
        concurrency::parallel_buffered_sort(v.begin(), v.end());
    });
    const bool okBuf = std::is_sorted(work.begin(), work.end()) && work == ref;

    std::cout << "## Lab 3, variant 6 (sorting std::vector<double>)\n\n";
    std::cout << "N = " << N << ", threads = " << std::thread::hardware_concurrency()
              << ", best of " << RUNS << " runs\n\n";
    std::cout << "| Algorithm | Time, s | Speedup vs std::sort | Result matches std::sort |\n";
    std::cout << "|---|---|---|---|\n";
    std::cout << std::fixed << std::setprecision(4);
    std::cout << "| std::sort | " << tStd << " | 1.00x | - |\n";
    std::cout << "| concurrency::parallel_sort | " << tPar << " | " << std::setprecision(2)
              << tStd / tPar << "x | " << (okPar ? "yes" : "NO") << " |\n" << std::setprecision(4);
    std::cout << "| concurrency::parallel_buffered_sort | " << tBuf << " | " << std::setprecision(2)
              << tStd / tBuf << "x | " << (okBuf ? "yes" : "NO") << " |\n";

    std::cout << "\nFirst x_n: " << std::setprecision(6);
    for (int n = 0; n < 5; ++n) std::cout << x[n] << (n < 4 ? ", " : "\n");
    std::cout << "Sorted: min = " << ref.front() << ", median = " << ref[N / 2]
              << ", max = " << ref.back() << "\n";
    return (okPar && okBuf) ? 0 : 1;
}
