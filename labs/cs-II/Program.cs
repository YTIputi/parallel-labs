using System;
using System.Diagnostics;
using System.Threading.Tasks;

class Program
{
    static double F(double x)
    {
        int M = Math.Max(20, (int)Math.Floor(20 * Math.Abs(x)));
        double x2 = x * x;

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

        F(1.0);

        var sw = Stopwatch.StartNew();
        double Sseq = 0;
        foreach (double x in X) Sseq += F(x);
        sw.Stop();
        double tSeq = sw.Elapsed.TotalSeconds;

        object locker = new object();
        double Slock = 0;
        sw.Restart();
        Parallel.ForEach(X, x =>
        {
            double tmp = F(x);
            lock (locker) { Slock += tmp; }
        });
        sw.Stop();
        double tLock = sw.Elapsed.TotalSeconds;

        double Slocal = 0;
        sw.Restart();
        Parallel.ForEach(
            X,
            () => 0.0,
            (x, loopState, local) => local + F(x),
            local => { lock (locker) { Slocal += local; } }
        );
        sw.Stop();
        double tLocal = sw.Elapsed.TotalSeconds;

        Console.WriteLine("## Task II (C#), variant 7: foreach vs Parallel.ForEach (array)\n");
        Console.WriteLine($"N = {N} (n = 0..{N}), processors = {Environment.ProcessorCount}\n");
        Console.WriteLine("| Version | S | Time, s | Speedup | Diff |");
        Console.WriteLine("|---|---|---|---|---|");
        Console.WriteLine($"| foreach | {Sseq:F10} | {tSeq:F4} | 1.00x | - |");
        Console.WriteLine($"| Parallel.ForEach + lock | {Slock:F10} | {tLock:F4} | {tSeq / tLock:F2}x | {Math.Abs(Slock - Sseq):E1} |");
        Console.WriteLine($"| Parallel.ForEach + localInit/localFinally | {Slocal:F10} | {tLocal:F4} | {tSeq / tLocal:F2}x | {Math.Abs(Slocal - Sseq):E1} |");
    }
}
