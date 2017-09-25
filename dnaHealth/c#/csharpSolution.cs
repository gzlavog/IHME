using System;
using System.Diagnostics;
using System.Collections.Generic;
using System.Text;
namespace dnahealth
{
    class Program
    {
      //builds the finite state machine
        static Dictionary<int, List<string>> buildMatchingMachine(Dictionary<string, Tuple<List<int>, List<long>>> genesandscores, int MAXS, out int[,] g, out int[] f,
            SortedSet<char> listofchars, out string s, int n)
        {
            int i, j, currentState, ch;
            int states = 1;

            g = new int[MAXS, listofchars.Count];
            for (i = 0; i < MAXS; ++i)
            {
                for (j = 0; j < listofchars.Count; ++j) g[i, j] = -1;
            }
            Dictionary<int, List<string>> outval = new Dictionary<int, List<string>>(n);
            if (listofchars.Count == 26) s = "abcdefghijklmnopqrstuvwxyz";
            else
            {
                StringBuilder mys = new StringBuilder();
                foreach (char c in listofchars) mys.Append(c);
                s = mys.ToString();
            }
            i = 0;
            foreach (string dictkey in genesandscores.Keys)
            {
                currentState = 0;
                // Insert all characters of current word in arr[]
                for (j = 0; j < dictkey.Length; ++j)
                {
                    ch = s.IndexOf(dictkey[j]);
                    // Allocate a new node (create a new state) if a
                    // node for ch doesn't exist.
                    if (g[currentState, ch] == -1) g[currentState, ch] = states++;
                    currentState = g[currentState, ch];
                }
                // Add current word in output function
                if (!outval.ContainsKey(currentState)) outval.Add(currentState, new List<string>() { dictkey });
                else outval[currentState].Add(dictkey);
                ++i;
            }
            //            Array.Resize(ref outval, states);
            f = new int[states];
            for (i = 0; i < states; ++i) f[i] = -1;
            // For all characters which don't have an edge from
            // root (or state 0) in Trie, add a goto edge to state
            // 0 itself
            for (ch = 0; ch < listofchars.Count; ++ch) if (g[0, ch] == -1) g[0, ch] = 0;
            Queue<int> q = new Queue<int>();
            for (ch = 0; ch < listofchars.Count; ++ch)
            {
                // All nodes of depth 1 have failure function value
                // as 0. For example, in above diagram we move to 0
                // from states 1 and 3.
                if (g[0, ch] != 0)
                {
                    f[g[0, ch]] = 0;
                    q.Enqueue(g[0, ch]);
                }
            }
            while (q.Count != 0)
            {
                // Remove the front state from queue
                int state = q.Dequeue();
                // For the removed state, find failure function for
                // all those characters for which goto function is
                // not defined.
                for (ch = 0; ch < listofchars.Count; ++ch)
                {
                    // If goto function is defined for character 'ch'
                    // and 'state'
                    if (g[state, ch] != -1)
                    {
                        // Find failure state of removed state
                        int failure = f[state];

                        // Find the deepest node labeled by proper
                        // suffix of string from root to current
                        // state.
                        while (g[failure, ch] == -1) failure = f[failure];
                        failure = g[failure, ch];
                        f[g[state, ch]] = failure;
                        // Merge output values
                        if (outval.ContainsKey(failure))
                        {
                            if (outval.ContainsKey(g[state, ch])) outval[g[state, ch]].AddRange(outval[failure]);
                            else outval.Add(g[state, ch], outval[failure]);
                        }
                        // Insert the next level node (of Trie) in Queue
                        q.Enqueue(g[state, ch]);
                    }
                }
            }
            return outval;
        }
        // Returns the next state the machine will transition to using goto
        // and failure functions.
        // currentState - The current state of the machine. Must be between
        //                0 and the number of states - 1, inclusive.
        // nextInput - The next character that enters into the machine.
        static int findNextState(int currentState, int ch, int[,] g, int[] f)
        {
            int answer = currentState;
            // If goto is not defined, use failure function
            while (g[answer, ch] == -1) answer = f[answer];
            return g[answer, ch];
        }
        static Dictionary<string, Tuple<List<int>, List<long>>> preparearrays(int lowestfirst, int highestlast, string[] genes_str,
            int[] healthvals, out int MAXS, out SortedSet<char> listofchars)
        {
            Dictionary<string, Tuple<List<int>, List<long>>> genesandscores = new Dictionary<string, Tuple<List<int>, List<long>>>();
            bool notfilled = true;
            MAXS = 0;
            listofchars = new SortedSet<char>();
            List<int> indexes;
            List<long> vals;
            Tuple<List<int>, List<long>> thist;
            for (int i = lowestfirst; i <= highestlast; ++i)
            {
                Debug.Assert(healthvals[i] < 10000001);
                if (healthvals[i] != 0)
                {
                    if (!genesandscores.ContainsKey(genes_str[i]))
                    {
                        indexes = new List<int>();
                        indexes.Add(i);
                        vals = new List<long>();
                        vals.Add(healthvals[i]);
                        genesandscores.Add(genes_str[i], Tuple.Create(indexes, vals));
                        if (notfilled)
                        {
                            foreach (char eachc in genes_str[i])
                            {
                                if (listofchars.Add(eachc) && listofchars.Count == 26) notfilled = false;
                            }
                        }
                        MAXS += genes_str[i].Length;
                    }
                    else
                    {
                        thist = genesandscores[genes_str[i]];
                        thist.Item1.Add(i);
                        thist.Item2.Add(thist.Item2[thist.Item2.Count - 1] + healthvals[i]);
                    }
                }
            }
            ++MAXS;
            return genesandscores;
        }
        // function returns the first bigger than valind or equal element in the list
        // with binary search complexity
        static int mylower_bound(List<int> myList, int valind)
        {
            if (myList == null) throw new ArgumentNullException("list");
            int lo = 0, hi = myList.Count - 1, mid;
            while (lo < hi)
            {
                mid = (lo + hi) >> 1;
                if (myList[mid] > valind) hi = mid - 1;
                else lo = mid + 1;
            }
            if (myList[lo] > valind) return --lo;
            return lo;
        }
        //searches the string text in the finite state machine
        static long searchWords(string text, int[] f, int[,] g, Dictionary<int, List<string>> outval, int first, int last, SortedSet<char> listofchars,
            Dictionary<string, Tuple<List<int>, List<long>>> genesandscores, string s)
        {
            int i, currentState = 0, foundlast;
            long sumval = 0;
            Tuple<List<int>, List<long>> thist;
            for (i = 0; i < text.Length; ++i)
            {
                if (listofchars.Contains(text[i]))
                {
                    currentState = findNextState(currentState, s.IndexOf(text[i]), g, f);
                    if (outval.ContainsKey(currentState))
                    {
                        foreach (string sinoutval in outval[currentState])
                        {
                            thist = genesandscores[sinoutval];
                            foundlast = thist.Item1[thist.Item1.Count - 1];
                            if (foundlast >= first && thist.Item1[0] <= last)
                            {
                                if (first != last)
                                {
                                    sumval += (foundlast <= last) ? thist.Item2[thist.Item2.Count - 1] : thist.Item2[mylower_bound(thist.Item1, last)];
                                    if (thist.Item1[0] < first) sumval -= thist.Item2[mylower_bound(thist.Item1, first - 1)];
                                }
                                else sumval += thist.Item2[mylower_bound(thist.Item1, last)];
                            }
                        }
                    }
                }
                else currentState = 0;
            }
            return sumval;
        }
        static void Main(string[] args)
        {
            int n = int.Parse(Console.ReadLine());
            //            Debug.Assert(n != 0 && n < 100001);
            string[] genes_str = new string[n];
            genes_str = Console.ReadLine().Split(' ');
            int[] healthvals = new int[n];
            healthvals = Array.ConvertAll(Console.ReadLine().Split(' '), int.Parse);
            int q = int.Parse(Console.ReadLine());
            //            Debug.Assert(q != 0 && q < 100001);

            string[][] line = new string[q][];
            int i, lowestfirst, highestlast;
            int[] first = new int[q];
            int[] last = new int[q];
            line[0] = Console.ReadLine().Split(' ');
            //            Debug.Assert(line[0].Length == 3);
            lowestfirst = first[0] = int.Parse(line[0][0]);
            highestlast = last[0] = int.Parse(line[0][1]);
            for (i = 1; i < q; ++i)
            {
                line[i] = Console.ReadLine().Split(' ');
                first[i] = int.Parse(line[i][0]);
                if (first[i] < lowestfirst) lowestfirst = first[i];
                last[i] = int.Parse(line[i][1]);
                //                Debug.Assert(first[i] <= last[i] && last[i] < n);
                if (last[i] > highestlast) highestlast = last[i];
            }
            int MAXS;
            SortedSet<char> listofchars;
            Dictionary<string, Tuple<List<int>, List<long>>> genesandscores = preparearrays(lowestfirst, highestlast, genes_str,
            healthvals, out MAXS, out listofchars);
            int[] f;
            int[,] g;
            string s;
            //builds finite state machine with given genes
            Dictionary<int, List<string>> outval = buildMatchingMachine(genesandscores, MAXS, out g, out f, listofchars, out s, n);
            long minstrhealth, maxstrhealth;
            long sumval = searchWords(line[0][2], f, g, outval, first[0], last[0], listofchars, genesandscores, s);
            minstrhealth = maxstrhealth = sumval;
            for (i = 1; i < q; ++i)
            {
                sumval = searchWords(line[i][2], f, g, outval, first[i], last[i], listofchars, genesandscores, s);
                if (sumval < minstrhealth) minstrhealth = sumval;
                else if (sumval > maxstrhealth) maxstrhealth = sumval;
            }
            Console.WriteLine("{0} {1}", minstrhealth, maxstrhealth);
        }
    }
}
