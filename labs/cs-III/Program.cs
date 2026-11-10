using System;
using System.Collections.Generic;
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

    static void RunTest(string name, IEnumerable<double> X)
    {
        var sw = Stopwatch.StartNew();
        double Sseq = 0;
        foreach (double x in X) Sseq += F(x);
        sw.Stop();
        double tSeq = sw.Elapsed.TotalSeconds;

        object locker = new object();
        double Spar = 0;
        sw.Restart();
        Parallel.ForEach(
            X,
            () => 0.0,
            (x, loopState, local) => local + F(x),
            local => { lock (locker) { Spar += local; } }
        );
        sw.Stop();
        double tPar = sw.Elapsed.TotalSeconds;

        Console.WriteLine($"| {name} | {Sseq:F10} | {Spar:F10} | {tSeq:F4} | {tPar:F4} | {tSeq / tPar:F2}x | {Math.Abs(Spar - Sseq):E1} |");
    }

    static void Main()
    {
        const int N = 200;

        var list = new List<double>();
        var stack = new Stack<double>();
        var queue = new Queue<double>();
        for (int n = 0; n <= N; n++)
        {
            double x = 100 * Math.Cos(n);
            list.Add(x);
            stack.Push(x);
            queue.Enqueue(x);
        }

        F(1.0);

        Console.WriteLine("## Task III (C#), variant 7: foreach vs Parallel.ForEach (List / Stack / Queue)\n");
        Console.WriteLine($"N = {N} (n = 0..{N}), processors = {Environment.ProcessorCount}\n");
        Console.WriteLine("| Container | S foreach | S Parallel.ForEach | T foreach, s | T Parallel.ForEach, s | Speedup | Diff |");
        Console.WriteLine("|---|---|---|---|---|---|---|");
        RunTest("List", list);
        RunTest("Stack", stack);
        RunTest("Queue", queue);
    }
}
