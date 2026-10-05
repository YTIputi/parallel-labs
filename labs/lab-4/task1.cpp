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

using T = unsigned int;
const int N = 50000000;
const int RUNS = 3;

double timeSort(const std::vector<T>& src, std::vector<T>& out,
                const std::function<void(std::vector<T>&)>& sortFn) {
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
    std::vector<T> x(N);
    for (int n = 0; n < N; ++n) {
        const double n2 = static_cast<double>(n) * n;
        x[n] = static_cast<T>(4e9 * (n2 / (1.0 + n2)) * std::fabs(std::sin(static_cast<double>(n))));
    }

    std::vector<T> ref, work;
    const double tStd = timeSort(x, ref, [](std::vector<T>& v) {
        std::sort(v.begin(), v.end());
    });

    struct Row { std::string name; double t; bool ok; };
    std::vector<Row> rows;
    auto check = [&](const std::string& name, const std::function<void(std::vector<T>&)>& fn) {
        const double t = timeSort(x, work, fn);
        rows.push_back({name, t, std::is_sorted(work.begin(), work.end()) && work == ref});
    };
    check("concurrency::parallel_sort", [](std::vector<T>& v) {
        concurrency::parallel_sort(v.begin(), v.end());
    });
    check("concurrency::parallel_buffered_sort", [](std::vector<T>& v) {
        concurrency::parallel_buffered_sort(v.begin(), v.end());
    });
    check("concurrency::parallel_radixsort", [](std::vector<T>& v) {
        concurrency::parallel_radixsort(v.begin(), v.end());
    });

    std::cout << "## Lab 4, variant 6 (sorting std::vector<unsigned int>)\n\n";
    std::cout << "N = " << N << ", threads = " << std::thread::hardware_concurrency()
              << ", best of " << RUNS << " runs\n\n";
    std::cout << "| Algorithm | Time, s | Speedup vs std::sort | Result matches std::sort |\n";
    std::cout << "|---|---|---|---|\n";
    std::cout << std::fixed << std::setprecision(4) << "| std::sort | " << tStd << " | 1.00x | - |\n";
    bool allOk = true;
    for (const auto& r : rows) {
        std::cout << "| " << r.name << " | " << std::setprecision(4) << r.t << " | "
                  << std::setprecision(2) << tStd / r.t << "x | " << (r.ok ? "yes" : "NO") << " |\n";
        allOk = allOk && r.ok;
    }

    std::cout << "\nFirst x_n: ";
    for (int n = 0; n < 5; ++n) std::cout << x[n] << (n < 4 ? ", " : "\n");
    std::cout << "Sorted: min = " << ref.front() << ", median = " << ref[N / 2]
              << ", max = " << ref.back() << "\n";
    return allOk ? 0 : 1;
}
