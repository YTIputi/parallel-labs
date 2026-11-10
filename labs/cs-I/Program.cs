// Задание I (C#), вариант 7
// Заполнить массив Y значениями f(x_n), x_n = 100 cos n, n = 0..N, N = 200.
// Сравнить оператор for и метод Parallel.For класса System.Threading.Tasks.Parallel.
//
// f(x) = sum_{k=1}^{M} sum_{j=1}^{M} (x^2 + x)(k - j) / (x^2 + k^3 + j^3) * sin(kx) * cos(jx),
// M = max(20, [20|x|])

using System;
using System.Diagnostics;
using System.Threading.Tasks;

class Program
{
    // Замер: один прогревочный прогон (JIT, запуск потоков пула) + R замеров, берётся медиана
    const int R = 5;
    static double Measure(Action body)
    {
        body();
        double[] t = new double[R];
        var sw = new Stopwatch();
        for (int r = 0; r < R; r++)
        {
            sw.Restart();
            body();
            sw.Stop();
            t[r] = sw.Elapsed.TotalSeconds;
        }
        Array.Sort(t);
        return t[R / 2];
    }

    static double F(double x)
    {
        int M = Math.Max(20, (int)Math.Floor(20 * Math.Abs(x)));
        double x2 = x * x;

        // sin(kx), cos(jx) и k^3 зависят только от одного индекса — считаем их заранее
        double[] s = new double[M + 1], c = new double[M + 1], cube = new double[M + 1];
        for (int i = 1; i <= M; i++)
        {
            s[i] = Math.Sin(i * x);
            c[i] = Math.Cos(i * x);
            cube[i] = (double)i * i * i;
        }

        double sum = 0;
        for (int k = 1; k <= M; k++)
        {
            double inner = 0;
            for (int j = 1; j <= M; j++)
                inner += (k - j) / (x2 + cube[k] + cube[j]) * c[j];
            sum += inner * s[k];
        }
        return (x2 + x) * sum;
    }

    static void Main()
    {
        const int N = 200;
        double[] X = new double[N + 1];
        for (int n = 0; n <= N; n++) X[n] = 100 * Math.Cos(n);

        double[] Yseq = new double[N + 1];
        double[] Ypar = new double[N + 1];

        // Последовательно: оператор for
        double tSeq = Measure(() =>
        {
            for (int n = 0; n <= N; n++)
                Yseq[n] = F(X[n]);
        });

        // Параллельно: Parallel.For (верхняя граница не включается, поэтому N + 1)
        double tPar = Measure(() =>
        {
            Parallel.For(0, N + 1, n =>
            {
                Ypar[n] = F(X[n]);   // каждая итерация пишет в свою ячейку — гонки нет
            });
        });

        double maxDiff = 0;
        for (int n = 0; n <= N; n++)
            maxDiff = Math.Max(maxDiff, Math.Abs(Yseq[n] - Ypar[n]));

        Console.WriteLine("## Task I (C#), variant 7: for vs Parallel.For\n");
        Console.WriteLine($"N = {N} (n = 0..{N}), processors = {Environment.ProcessorCount}, time = median of {R} runs\n");
        Console.WriteLine("| Version | Time, s |\n|---|---|");
        Console.WriteLine($"| for | {tSeq:F4} |");
        Console.WriteLine($"| Parallel.For | {tPar:F4} |\n");
        Console.WriteLine($"Speedup: **{tSeq / tPar:F2}x**\n");
        Console.WriteLine($"Max |Y_seq - Y_par| = {maxDiff:E1}\n");
        Console.WriteLine("First values:\n\n| n | x_n | y_n |\n|---|---|---|");
        for (int n = 0; n < 5; n++)
            Console.WriteLine($"| {n} | {X[n]:F6} | {Yseq[n]:F6} |");
    }
}
