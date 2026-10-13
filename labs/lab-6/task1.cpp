#include <iostream>
#include <iomanip>
#include <vector>
#include <cmath>
#include <chrono>
#include <algorithm>
#include <thread>
#include <ppl.h>
#include <concurrent_vector.h>
#include <concurrent_queue.h>

const int N1 = 400;
const int N2 = 50;
const double PI = 3.14159265358979323846;

struct Point { double x, y, f; };

double W[N2 + 1][N2 + 1];

void initW() {
    for (int k = 1; k <= N2; ++k)
        for (int j = 1; j <= N2; ++j) {
            const double k4 = std::pow(k, 4.0), j4 = std::pow(j, 4.0);
            W[k][j] = 1.0 / ((1.0 + k + j) * std::sqrt(1.0 + k4 + j4));
        }
}

bool evalPoint(double x, double y, double& f) {
    double ck[N2 + 1], sj[N2 + 1], jcj[N2 + 1];
    for (int i = 1; i <= N2; ++i) {
        ck[i]  = std::cos(i * x);
        sj[i]  = std::sin(i * y);
        jcj[i] = i * std::cos(i * y);
    }
    double fv = 0.0, dfdy = 0.0;
    for (int k = 1; k <= N2; ++k) {
        double a = 0.0, b = 0.0;
        for (int j = 1; j <= N2; ++j) {
            a += W[k][j] * sj[j];
            b += W[k][j] * jcj[j];
        }
        fv   += ck[k] * a;
        dfdy += ck[k] * b;
    }
    f = fv;
    return dfdy <= 0.0;
}

double nodeCoord(int i) { return 2.0 * PI * i / (N1 - 1); }

bool lessXY(const Point& a, const Point& b) {
    return a.x < b.x || (a.x == b.x && a.y < b.y);
}

double compare(std::vector<Point> ref, std::vector<Point> got, bool& sameSet) {
    std::sort(ref.begin(), ref.end(), lessXY);
    std::sort(got.begin(), got.end(), lessXY);
    sameSet = ref.size() == got.size();
    double d = 0.0;
    for (size_t i = 0; sameSet && i < ref.size(); ++i) {
        if (ref[i].x != got[i].x || ref[i].y != got[i].y) { sameSet = false; break; }
        d = std::max(d, std::fabs(ref[i].f - got[i].f));
    }
    return d;
}

using Clock = std::chrono::steady_clock;
double sec(Clock::time_point a, Clock::time_point b) {
    return std::chrono::duration<double>(b - a).count();
}

int main() {
    initW();

    auto t0 = Clock::now();
    std::vector<Point> seq;
    for (int i = 0; i < N1; ++i)
        for (int l = 0; l < N1; ++l) {
            const double x = nodeCoord(i), y = nodeCoord(l);
            double f;
            if (evalPoint(x, y, f)) seq.push_back({x, y, f});
        }
    auto t1 = Clock::now();
  
    concurrency::concurrent_vector<Point> cv;
    concurrency::parallel_for(0, N1, [&](int i) {
        const double x = nodeCoord(i);
        for (int l = 0; l < N1; ++l) {
            const double y = nodeCoord(l);
            double f;
            if (evalPoint(x, y, f)) cv.push_back({x, y, f});
        }
    });
    auto t2 = Clock::now();

    concurrency::concurrent_queue<Point> cq;
    concurrency::parallel_for(0, N1, [&](int i) {
        const double x = nodeCoord(i);
        for (int l = 0; l < N1; ++l) {
            const double y = nodeCoord(l);
            double f;
            if (evalPoint(x, y, f)) cq.push({x, y, f});
        }
    });
    auto t3 = Clock::now();

    const double tSeq = sec(t0, t1), tVec = sec(t1, t2), tQue = sec(t2, t3);

    std::vector<Point> fromVec(cv.begin(), cv.end());
    std::vector<Point> fromQue;
    Point p;
    while (cq.try_pop(p)) fromQue.push_back(p);

    bool okVec, okQue;
    const double dVec = compare(seq, fromVec, okVec);
    const double dQue = compare(seq, fromQue, okQue);

    std::cout << "## Task 6, variant 6 (concurrent_vector / concurrent_queue)\n\n";
    std::cout << "Grid " << N1 << " x " << N1 << " = " << N1 * N1 << " points, N2 = " << N2
              << ", threads = " << std::thread::hardware_concurrency() << "\n\n";
    std::cout << "Kept points (df/dy <= 0): " << seq.size() << " of " << N1 * N1 << " ("
              << std::fixed << std::setprecision(1) << 100.0 * seq.size() / (N1 * N1) << "%)\n\n";

    std::cout << "| Version | Container | Time, s | Speedup | Same points | Max diff f |\n";
    std::cout << "|---|---|---|---|---|---|\n";
    std::cout << std::fixed << std::setprecision(4);
    std::cout << "| Sequential | std::vector | " << tSeq << " | 1.00x | - | - |\n";
    std::cout << "| parallel_for | concurrent_vector | " << tVec << " | " << std::setprecision(2)
              << tSeq / tVec << "x | " << (okVec ? "yes" : "NO") << " | "
              << std::scientific << std::setprecision(1) << dVec << " |\n";
    std::cout << std::fixed << std::setprecision(4);
    std::cout << "| parallel_for | concurrent_queue | " << tQue << " | " << std::setprecision(2)
              << tSeq / tQue << "x | " << (okQue ? "yes" : "NO") << " | "
              << std::scientific << std::setprecision(1) << dQue << " |\n\n";

    std::sort(seq.begin(), seq.end(), lessXY);
    std::cout << std::fixed << std::setprecision(6);
    std::cout << "First kept points:\n\n| x | y | f(x,y) |\n|---|---|---|\n";
    for (size_t i = 0; i < 5 && i < seq.size(); ++i)
        std::cout << "| " << seq[i].x << " | " << seq[i].y << " | " << seq[i].f << " |\n";
    return 0;
}
