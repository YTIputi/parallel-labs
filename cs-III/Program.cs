// Задание III (C#), вариант 7
// То же, что II, но в качестве X используются контейнеры List, Stack и Queue .NET.
// x_n = 100 cos n, n = 0..N, N = 200. Вычислить S = sum_{n=0}^{N} f(x_n).
// Сравнить операцию foreach и метод Parallel.ForEach.
//
// f(x) = sum_{k=1}^{M} sum_{j=1}^{M} (x^2 + x)(k - j) / (x^2 + k^3 + j^3) * sin(kx) * cos(jx),
// M = max(20, [20|x|])

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

    // Один и тот же тест для любого контейнера: List, Stack и Queue
    // все реализуют интерфейс IEnumerable<double>, по которому работают и foreach, и Parallel.ForEach.
    static void RunTest(string name, IEnumerable<double> X)
    {
        // Последовательно: foreach
        var sw = Stopwatch.StartNew();
        double Sseq = 0;
        foreach (double x in X) Sseq += F(x);
        sw.Stop();
        double tSeq = sw.Elapsed.TotalSeconds;

        // Параллельно: Parallel.ForEach с локальной суммой потока
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
            list.Add(x);       // в конец
            stack.Push(x);     // на вершину (обход будет в обратном порядке)
            queue.Enqueue(x);  // в конец очереди
        }

        F(1.0);   // прогрев JIT

        Console.WriteLine("## Task III (C#), variant 7: foreach vs Parallel.ForEach (List / Stack / Queue)\n");
        Console.WriteLine($"N = {N} (n = 0..{N}), processors = {Environment.ProcessorCount}\n");
        Console.WriteLine("| Container | S foreach | S Parallel.ForEach | T foreach, s | T Parallel.ForEach, s | Speedup | Diff |");
        Console.WriteLine("|---|---|---|---|---|---|---|");
        RunTest("List", list);
        RunTest("Stack", stack);
        RunTest("Queue", queue);
    }
}
