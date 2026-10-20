#include <iostream>
#include <iomanip>
#include <vector>
#include <cmath>
#include <chrono>
#include <functional>
#include <thread>
#include <ppl.h>

const double A = 0.0, B = 1600.0;
const int N = 200000;

const int KMAX = 2000;
double EK[KMAX];

double f(double x, double y) {
    const double r = std::sqrt(x * x + y * y);
    double p = 1.0;
    for (int k = 0; k < KMAX; ++k) {
        const double u = x / (r + EK[k]);
        if (std::fabs(u) < 1e-9) break;
        p *= std::cos(u);
    }
    const double s = std::sqrt(std::fabs(x));
    return p / (std::exp(-s) + std::exp(s));
}

double simpsonSeq(double y) {
    const double h = (B - A) / (2.0 * N);
    double s4 = 0.0, s2 = 0.0;
    for (int k = 1; k <= N; ++k)     s4 += f(A + (2 * k - 1) * h, y);
    for (int k = 1; k <= N - 1; ++k) s2 += f(A + 2 * k * h, y);
    return h / 3.0 * (f(A, y) + f(B, y) + 4.0 * s4 + 2.0 * s2);
}

double simpsonPar(double y) {
    const double h = (B - A) / (2.0 * N);
    concurrency::combinable<double> s4([] { return 0.0; });
    concurrency::combinable<double> s2([] { return 0.0; });

    concurrency::parallel_for(1, N + 1, [&](int k) {
        s4.local() += f(A + (2 * k - 1) * h, y);
    });
    concurrency::parallel_for(1, N, [&](int k) {
        s2.local() += f(A + 2 * k * h, y);
    });

    const double sum4 = s4.combine(std::plus<double>());
    const double sum2 = s2.combine(std::plus<double>());
    return h / 3.0 * (f(A, y) + f(B, y) + 4.0 * sum4 + 2.0 * sum2);
}

using Clock = std::chrono::steady_clock;
double sec(Clock::time_point a, Clock::time_point b) {
    return std::chrono::duration<double>(b - a).count();
}

int main() {
    for (int k = 0; k < KMAX; ++k) EK[k] = std::exp(std::sqrt(static_cast<double>(k)));

    const std::vector<double> ys = { 0.0, 1.0, 2.0, 3.0, 4.0, 5.0 };
    std::vector<double> Fseq(ys.size()), Fpar(ys.size()), tSeq(ys.size()), tPar(ys.size());

    for (size_t i = 0; i < ys.size(); ++i) {
        auto t0 = Clock::now();
        Fseq[i] = simpsonSeq(ys[i]);
        auto t1 = Clock::now();
        Fpar[i] = simpsonPar(ys[i]);
        auto t2 = Clock::now();
        tSeq[i] = sec(t0, t1);
        tPar[i] = sec(t1, t2);
    }

    double totalSeq = 0.0, totalPar = 0.0;
    for (size_t i = 0; i < ys.size(); ++i) { totalSeq += tSeq[i]; totalPar += tPar[i]; }

    std::cout << "## Lab VII, variant 3 (Simpson + concurrency::combinable)\n\n";
    std::cout << "Interval [" << A << ", " << B << "], N = " << N << " (" << 2 * N + 1
              << " nodes), threads = " << std::thread::hardware_concurrency() << "\n\n";
    std::cout << "| y | F(y) sequential | F(y) combinable | Diff | T seq, s | T par, s | Speedup |\n";
    std::cout << "|---|---|---|---|---|---|---|\n";
    for (size_t i = 0; i < ys.size(); ++i) {
        std::cout << std::fixed << std::setprecision(1) << "| " << ys[i]
                  << " | " << std::setprecision(12) << Fseq[i]
                  << " | " << Fpar[i]
                  << " | " << std::scientific << std::setprecision(1) << std::fabs(Fseq[i] - Fpar[i])
                  << " | " << std::fixed << std::setprecision(4) << tSeq[i]
                  << " | " << tPar[i]
                  << " | " << std::setprecision(2) << tSeq[i] / tPar[i] << "x |\n";
    }
    std::cout << "\nTotal: sequential " << std::setprecision(4) << totalSeq << " s, combinable "
              << totalPar << " s, speedup **" << std::setprecision(2) << totalSeq / totalPar << "x**\n";
    return 0;
}
