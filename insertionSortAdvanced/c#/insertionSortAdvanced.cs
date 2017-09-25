using System;
using System.Collections.Generic;

namespace insertionsort_advanced
{
    class Program
    {
        static void getSum(ulong[] BITree, int index, ref ulong sum)
        {
            // loop through the ancestors of BITree[index]
            while (index > 0)
            {
                // Add current element of BITree to sum
                sum += BITree[index];
                index -= index & (-index);
            }
        }
        static int convert(int[] arr, int n)
        {
            // Create a copy of arrp[] in temp and sort the temp array
            // in increasing order
            SortedDictionary<int, List<int>> mydict = new SortedDictionary<int, List<int>>();
            mydict.Add(arr[0], new List<int> { 0 });
            List<int> addlist;
            int i;
            for (i = 1; i < n; ++i)
            {
                if (mydict.TryGetValue(arr[i], out addlist)) addlist.Add(i);
                else mydict.Add(arr[i], new List<int> { i });
            }
            i = 1;
            foreach (List<int> l in mydict.Values)
            {
                foreach (int j in l) arr[j] = i++;
            }
            return i;
        }
        // Updates a node in Binary Index Tree (BITree) at given index
        // in BITree.  The given value 'val' is added to BITree[i] and
        // all of its ancestors in tree.
        static void updateBIT(ulong[] BITree, int n, int index)
        {
            // Traverse all ancestors of BITree[index] and add 1
            while (index <= n)
            {
                ++BITree[index];
                index += index & (-index);
            }
        }
        static void Main(string[] args)
        {
            byte T = byte.Parse(Console.ReadLine());
            int N;
            int[] arr;
            while (T-- != 0)
            {
                N = int.Parse(Console.ReadLine());
                arr = Array.ConvertAll(Console.ReadLine().Split(), int.Parse);
                ulong[] BIT = new ulong[convert(arr, N)];
                ulong invcount = 0;
                for (int i = N - 1; i > 0; i--)
                {
                    getSum(BIT, arr[i] - 1, ref invcount);
                    updateBIT(BIT, N, arr[i]);
                }
                getSum(BIT, arr[0] - 1, ref invcount);
                Console.WriteLine(invcount);
            }
        }
    }
}