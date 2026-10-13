#include <iostream>
#include <iomanip>
#include <vector>
#include <cmath>
#include <chrono>
#include <algorithm>
#include <thread>
#include <agents.h>

const int NX = 800, NY = 800;
const double XMIN = -5.0, XMAX = 5.0;
const int K = 500, KH = K / 2;

double EK[K];

struct Row {
    int i;
    double x;
    std::vector<double> v;
};

double coord(int i, int n) { return XMIN + (XMAX - XMIN) * i / (n - 1); }

void stage1(Row& r) {
    for (int j = 0; j < NY; ++j) {
        const double y = coord(j, NY);
        const double c = std::cos(r.x + y), c2 = c * c;
        double p = 1.0;
        for (int k = 0; k < KH; ++k)
            p *= std::cos(r.x / (c2 + EK[k]));
        r.v[j] = p;
    }
}

void stage2(Row& r) {
    const double factor = std::exp(-std::sqrt(std::fabs(r.x)));
    for (int j = 0; j < NY; ++j) {
        const double y = coord(j, NY);
        const double c = std::cos(r.x + y), c2 = c * c;
        double p = r.v[j];
        for (int k = KH; k < K; ++k)
            p *= std::cos(r.x / (c2 + EK[k]));
        r.v[j] = factor * p;
    }
}

std::vector<Row> makeRows() {
    std::vector<Row> rows(NX);
    for (int i = 0; i < NX; ++i) rows[i] = { i, coord(i, NX), std::vector<double>(NY) };
    return rows;
}

class StageAgent : public concurrency::agent {
public:
    StageAgent(concurrency::ISource<Row*>& in, concurrency::ITarget<Row*>& out, void (*fn)(Row&))
        : in_(in), out_(out), fn_(fn) {}
protected:
    void run() override {
        for (;;) {
            Row* r = concurrency::receive(in_);
            if (r == nullptr) { concurrency::send(out_, r); break; }
            fn_(*r);
            concurrency::send(out_, r);
        }
        done();
    }
private:
    concurrency::ISource<Row*>& in_;
    concurrency::ITarget<Row*>& out_;
    void (*fn_)(Row&);
};

using Clock = std::chrono::steady_clock;
double sec(Clock::time_point a, Clock::time_point b) {
    return std::chrono::duration<double>(b - a).count();
}

double maxDiff(const std::vector<Row>& a, const std::vector<Row>& b) {
    double d = 0.0;
    for (int i = 0; i < NX; ++i)
        for (int j = 0; j < NY; ++j)
            d = std::max(d, std::fabs(a[i].v[j] - b[i].v[j]));
    return d;
}

int main() {
    for (int k = 0; k < K; ++k) EK[k] = std::exp(std::sqrt(static_cast<double>(k)));

    std::vector<Row> rSeq = makeRows(), rAg = makeRows(), rTr = makeRows();

    auto t0 = Clock::now();
    for (Row& r : rSeq) { stage1(r); stage2(r); }
    auto t1 = Clock::now();

    {
        concurrency::unbounded_buffer<Row*> buf1, buf2, buf3;
        StageAgent a1(buf1, buf2, stage1);
        StageAgent a2(buf2, buf3, stage2);
        a1.start();
        a2.start();

        for (Row& r : rAg) concurrency::send(buf1, &r);
        concurrency::send(buf1, static_cast<Row*>(nullptr));

        while (concurrency::receive(buf3) != nullptr) {}
        concurrency::agent::wait(&a1);
        concurrency::agent::wait(&a2);
    }
    auto t2 = Clock::now();

    {
        concurrency::transformer<Row*, Row*> tf1([](Row* r) -> Row* { stage1(*r); return r; });
        concurrency::transformer<Row*, Row*> tf2([](Row* r) -> Row* { stage2(*r); return r; });
        concurrency::unbounded_buffer<Row*> out;
        tf1.link_target(&tf2);
        tf2.link_target(&out);

        for (Row& r : rTr) concurrency::send(tf1, &r);
        for (int i = 0; i < NX; ++i) concurrency::receive(out);
    }
    auto t3 = Clock::now();

    const double tSeq = sec(t0, t1), tAg = sec(t1, t2), tTr = sec(t2, t3);

    std::cout << "## Task 7, variant 18 (pipeline f = f2 o f1)\n\n";
    std::cout << "Grid " << NX << " x " << NY << " on [" << XMIN << ", " << XMAX << "]^2, K = " << K
              << " factors (" << KH << " per stage), threads = "
              << std::thread::hardware_concurrency() << "\n\n";
    std::cout << "| Version | Time, s | Speedup | Max diff |\n|---|---|---|---|\n";
    std::cout << std::fixed << std::setprecision(4)
              << "| Sequential | " << tSeq << " | 1.00x | - |\n"
              << "| a) unbounded_buffer + agents | " << tAg << " | " << std::setprecision(2) << tSeq / tAg
              << "x | " << std::scientific << std::setprecision(1) << maxDiff(rSeq, rAg) << " |\n"
              << std::fixed << std::setprecision(4)
              << "| b) transformer | " << tTr << " | " << std::setprecision(2) << tSeq / tTr
              << "x | " << std::scientific << std::setprecision(1) << maxDiff(rSeq, rTr) << " |\n\n";

    std::cout << std::fixed << std::setprecision(6)
              << "Sample values:\n\n| x | y | f(x,y) |\n|---|---|---|\n";
    const int idx[][2] = { {0, 0}, {NX / 4, NY / 2}, {NX / 2, NY / 3}, {3 * NX / 4, NY - 1}, {NX - 1, NY / 5} };
    for (auto& q : idx)
        std::cout << "| " << rSeq[q[0]].x << " | " << coord(q[1], NY) << " | " << rSeq[q[0]].v[q[1]] << " |\n";
    return 0;
}
