using System;
using System.Collections.Generic;
using System.Collections;
using System.Linq;

namespace gridland_provinces
{
    class Program
    {
        const ulong prime = 4294967291;
        //builds the alphabet composed of the letters found in strings s1 and s2
        //adds to rolling hash dictionary with fixed length words starting at each possible position
        static byte getalphtopower(ref string s1, ref string s2, ushort ncolumns, ushort twoncolumns, out Dictionary<char, ulong[]> alphtopower, out char firstchar)
        {
            BitArray mybitset = new BitArray(26);
            ushort i;
            for (i = 0; i < ncolumns; ++i)
            {
                mybitset.Set(s1[i] - 'a', true);
                mybitset.Set(s2[i] - 'a', true);
            }
            List<char> vofchars = new List<char>();
            for (i = 0; i < 26; ++i) if (mybitset.Get(i)) vofchars.Add((char)('a' + i));
            byte alphabetsize = (byte)vofchars.Count;
            firstchar = vofchars[0];
            alphtopower = new Dictionary<char, ulong[]>();
            if (alphabetsize != 1)
            {
                if (ncolumns == 1) alphabetsize = 2;
                else
                {
                    alphtopower[firstchar] = new ulong[twoncolumns];
                    alphtopower[firstchar][0] = 1;
                    alphtopower[firstchar][1] = alphabetsize;
                    //rolling hash build
                    for (i = 2; i < twoncolumns; ++i) alphtopower[firstchar][i] = (alphtopower[firstchar][i - 1] * alphabetsize) % prime;
                    for (byte j = 1; j < alphabetsize; ++j)
                    {
                        alphtopower[vofchars[j]] = new ulong[twoncolumns];
                    //rolling hash build
                        for (i = 0; i < twoncolumns; ++i) alphtopower[vofchars[j]][i] = (alphtopower[vofchars[j - 1]][i] + alphtopower[firstchar][i]) % prime;
                    }
                }
            }
            return alphabetsize;
        }
        static void RabinKarpArray(ref Dictionary<char, ulong[]> alphtopower, ushort ncolumns, ref string s1, ref string s2, out ulong[] hasharray1,
            out ulong[] revhasharray1, out ulong[] hasharray2, out ulong[] revhasharray2, byte alphabetsize, ushort ncolm1)
        {
            hasharray1 = new ulong[ncolumns];
            hasharray1[ncolm1] = alphtopower[s1[ncolm1]][2] + alphtopower[s2[ncolm1]][1] + alphtopower[s2[ncolumns - 2]][0];
            hasharray2 = new ulong[ncolumns];
            hasharray2[ncolm1] = alphtopower[s2[ncolm1]][2] + alphtopower[s1[ncolm1]][1] + alphtopower[s1[ncolumns - 2]][0];
            revhasharray1 = new ulong[ncolm1];
            revhasharray1[ncolm1 - 1] = alphtopower[s1[ncolm1]][0] + alphtopower[s2[ncolm1]][1] + alphtopower[s2[ncolumns - 2]][2];
            revhasharray2 = new ulong[ncolm1];
            revhasharray2[ncolm1 - 1] = alphtopower[s2[ncolm1]][0] + alphtopower[s1[ncolm1]][1] + alphtopower[s1[ncolumns - 2]][2];
            for (ushort i = (ushort)(ncolm1 - 1); i != 0; --i)
            {
                hasharray1[i] = (hasharray1[i + 1] * alphabetsize + alphtopower[s1[i]][(ncolumns - i) << 1] + alphtopower[s2[i - 1]][0]) % prime;
                hasharray2[i] = (hasharray2[i + 1] * alphabetsize + alphtopower[s2[i]][(ncolumns - i) << 1] + alphtopower[s1[i - 1]][0]) % prime;
                revhasharray1[i - 1] = (revhasharray1[i] * alphabetsize + alphtopower[s1[i]][0] + alphtopower[s2[i - 1]][(ncolumns - i) << 1]) % prime;
                revhasharray2[i - 1] = (revhasharray2[i] * alphabetsize + alphtopower[s2[i]][0] + alphtopower[s1[i - 1]][(ncolumns - i) << 1]) % prime;
            }
            hasharray1[0] = (hasharray1[1] + alphtopower[s1[0]][(ncolumns << 1) - 1]) % prime;
            hasharray2[0] = (hasharray2[1] + alphtopower[s2[0]][(ncolumns << 1) - 1]) % prime;
        }
        static void getlastnodehashes(ref Dictionary<char, ulong[]> alphtopower, ushort ncolumns, ref string s1, ref string s2,
            ref Dictionary<ulong, ulong> myhashd, ref Dictionary<ulong, HashSet<ulong>> collisions, ushort ncolm1)
        {
            ulong result = alphtopower[s2[ncolm1]][0];
            ulong revresult = alphtopower[s1[ncolm1]][0];
            int i;
            for (i = ncolm1 - 1; i != -1; --i)
            {
                result += alphtopower[s2[i]][ncolm1 - i];
                revresult += alphtopower[s1[i]][ncolm1 - i];
            }
            for (i = 0; i < ncolumns; ++i)
            {
                result += alphtopower[s1[i]][ncolumns + i];
                revresult += alphtopower[s2[i]][ncolumns + i];
            }
            AddtoDict(result % prime, revresult % prime, ref myhashd, ref collisions);
        }
        //builds(adds to) the dictionary of hashes of strings seen so far
        static void AddtoDict(ulong check, ulong revcheck, ref Dictionary<ulong, ulong> myhashd, ref Dictionary<ulong, HashSet<ulong>> collisions, bool addreverse = true)
        {
            if (!myhashd.ContainsKey(check))
            {
                myhashd.Add(check, revcheck);
                if (addreverse) AddtoDict(revcheck, check, ref myhashd, ref collisions, false);
            }
            else if (!collisions.ContainsKey(check))
            {
                collisions.Add(check, new HashSet<ulong> { myhashd[check] });
                if (collisions[check].Add(revcheck) && addreverse) AddtoDict(revcheck, check, ref myhashd, ref collisions, false);
            }
            else if (collisions[check].Add(revcheck) && addreverse) AddtoDict(revcheck, check, ref myhashd, ref collisions, false);
        }
        static int returnResult(ref Dictionary<ulong, HashSet<ulong>> collisions)
        {
            int result = 0;
            foreach (KeyValuePair<ulong, HashSet<ulong>> entry in collisions) result += entry.Value.Count - 1;
            return result;
        }
        //gooes through all possible ways of building the string and makes a dictionary of hashes for all strings built, calculating an int hashvalue for each string.
        // uses Rabin-Karp algorithm to build the string hash
        static int distinctwaysRK(ushort ncolumns, ref string ss1, ref string ss2)
        {
            Dictionary<char, ulong[]> alphtopower;
            ushort twoncolumns = (ushort)(ncolumns << 1);
            char firstchar;
            byte alphabetsize = getalphtopower(ref ss1, ref ss2, ncolumns, twoncolumns, out alphtopower, out firstchar);
            if (alphabetsize == 1) return 1;
            if (ncolumns == 1) return 2;
            ushort ncolsm1 = (ushort)(ncolumns - 1);
            ulong[] goroundup, rev_goroundup, gorounddown, rev_gorounddown;
            //there are multiple ways to build the final string: 1.going right in the first string (goroundup) + snaking up-down till touching all remaining nodes)
            // or                                                2. like in but 1 in reverse
            RabinKarpArray(ref alphtopower, ncolumns, ref ss1, ref ss2, out goroundup, out rev_goroundup, out gorounddown, out rev_gorounddown, alphabetsize, ncolsm1);
            //Start at Node 0;
            Dictionary<ulong, ulong> myhashd = new Dictionary<ulong, ulong>();
            myhashd.Add(goroundup[0], gorounddown[0]);
            Dictionary<ulong, HashSet<ulong>> collisions = new Dictionary<ulong, HashSet<ulong>>();
            AddtoDict(gorounddown[0], goroundup[0], ref myhashd, ref collisions, false);
            //Node 1
            AddtoDict((goroundup[1] * alphabetsize + alphtopower[ss1[0]][0]) % prime, (rev_goroundup[0] + alphtopower[ss1[0]][twoncolumns - 1]) % prime, ref myhashd, ref collisions);
            AddtoDict((gorounddown[1] * alphabetsize + alphtopower[ss2[0]][0]) % prime, (rev_gorounddown[0] + alphtopower[ss2[0]][twoncolumns - 1]) % prime, ref myhashd, ref collisions);
            //Lastnode
            getlastnodehashes(ref alphtopower, ncolumns, ref ss1, ref ss2, ref myhashd, ref collisions, ncolsm1);
            //Node 2 up
            if (ncolumns == 2) return myhashd.Count + returnResult(ref collisions);
            List<ulong>[] proposals = new List<ulong>[2] { new List<ulong>() { alphtopower[ss1[1]][2] + alphtopower[ss1[0]][1] + alphtopower[ss2[0]][0] },
            new List<ulong>() { alphtopower[ss2[1]][2] + alphtopower[ss2[0]][1] + alphtopower[ss1[0]][0] } };
            List<ulong>[] revproposals = new List<ulong>[2] { new List<ulong>() { alphtopower[ss2[0]][2] + alphtopower[ss1[0]][1] + alphtopower[ss1[1]][0] },
            new List<ulong>() { alphtopower[ss1[0]][2] + alphtopower[ss2[0]][1] + alphtopower[ss2[1]][0] } };
            ulong temp1 = goroundup[2] * alphtopower[firstchar][3];
            AddtoDict((temp1 + proposals[0][0]) % prime, (rev_goroundup[1] + revproposals[0][0] * alphtopower[firstchar][twoncolumns - 3]) % prime, ref myhashd, ref collisions);
            if (ss2[0] != ss1[1])
            {
                proposals[0].Add(revproposals[0][0]);
                revproposals[0].Add(proposals[0][0]);
                AddtoDict((temp1 + proposals[0][1]) % prime, (rev_goroundup[1] + revproposals[0][1] * alphtopower[firstchar][twoncolumns - 3]) % prime, ref myhashd, ref collisions);
            }
            //Node 2 down
            temp1 = gorounddown[2] * alphtopower[firstchar][3];
            AddtoDict((temp1 + proposals[1][0]) % prime, (rev_gorounddown[1] + revproposals[1][0] * alphtopower[firstchar][twoncolumns - 3]) % prime, ref myhashd, ref collisions);
            if (ss1[0] != ss2[1])
            {
                proposals[1].Add(revproposals[1][0]);
                revproposals[1].Add(proposals[1][0]);
                AddtoDict((temp1 + proposals[1][1]) % prime, (rev_gorounddown[1] + revproposals[1][1] * alphtopower[firstchar][twoncolumns - 3]) % prime, ref myhashd, ref collisions);
            }
            //Node 3 and up
            //		cout << myhashd.Count << endl;
            if (ncolumns == 3) return myhashd.Count + returnResult(ref collisions);
            ushort i, j, twoim2, twoim1, twontwoi;
            bool notfound;
            ulong candidate0, candidate1, revcandidate1;
            List<ulong> temp = new List<ulong>();
            for (i = 3; i < ncolsm1; ++i)
            {
                twoim2 = (ushort)((i - 1) << 1);
                twoim1 = (ushort)(twoim2 + 1);
                candidate1 = (proposals[1].Last() * alphabetsize + alphtopower[ss1[i - 2]][twoim2] + alphtopower[ss2[i - 1]][0]) % prime;
                revcandidate1 = (revproposals[1].Last() * alphabetsize + alphtopower[ss2[i - 1]][twoim2] + alphtopower[ss1[i - 2]][0]) % prime;
                candidate0 = (proposals[0].Last() * alphabetsize + alphtopower[ss2[i - 2]][twoim2] + alphtopower[ss1[i - 1]][0]) % prime;
                notfound = true;
                temp1 = alphtopower[ss1[i - 1]][twoim2] + alphtopower[ss1[i - 2]][twoim2 - 1];
                for (j = 0; j < proposals[1].Count; ++j)
                {
                    if (notfound)
                    {
                        proposals[1][j] = (proposals[1][j] + temp1) % prime;
                        if (proposals[1][j] == candidate0) notfound = false;
                        revproposals[1][j] = (revproposals[1][j] * alphtopower[firstchar][2] + alphtopower[ss1[i - 2]][1] + alphtopower[ss1[i - 1]][0]) % prime;
                    }
                    else
                    {
                        proposals[1][j - 1] = (proposals[1][j] + temp1) % prime;
                        revproposals[1][j - 1] = (revproposals[1][j] * alphtopower[firstchar][2] + alphtopower[ss1[i - 2]][1] + alphtopower[ss1[i - 1]][0]) % prime;
                    }
                }
                if (notfound)
                {
                    proposals[1].Add(candidate0);
                    revproposals[1].Add((revproposals[0].Last() * alphabetsize + alphtopower[ss1[i - 1]][twoim2] + alphtopower[ss2[i - 2]][0]) % prime);
                }
                else
                {
                    proposals[1][j - 1] = candidate0;
                    revproposals[1][j - 1] = (revproposals[0].Last() * alphabetsize + alphtopower[ss1[i - 1]][twoim2] + alphtopower[ss2[i - 2]][0]) % prime;
                }
                temp1 = goroundup[i] * alphtopower[firstchar][twoim1];
                twontwoi = (ushort)(twoncolumns - twoim1);
                for (j = 0; j < proposals[1].Count; ++j)
                    AddtoDict((temp1 + proposals[1][j]) % prime, (rev_goroundup[i - 1] + revproposals[1][j] * alphtopower[firstchar][twontwoi]) % prime, ref myhashd, ref collisions);
                notfound = true;
                temp1 = alphtopower[ss2[i - 1]][twoim2] + alphtopower[ss2[i - 2]][twoim2 - 1];
                for (j = 0; j < proposals[0].Count; ++j)
                {
                    if (notfound)
                    {
                        proposals[0][j] = (proposals[0][j] + temp1) % prime;
                        if (proposals[0][j] == candidate1) notfound = false;
                        else revproposals[0][j] = (revproposals[0][j] * alphtopower[firstchar][2] + alphtopower[ss2[i - 2]][1] + alphtopower[ss2[i - 1]][0]) % prime;
                    }
                    else
                    {
                        proposals[0][j - 1] = (proposals[0][j] + temp1) % prime;
                        revproposals[0][j - 1] = (revproposals[0][j] * alphtopower[firstchar][2] + alphtopower[ss2[i - 2]][1] + alphtopower[ss2[i - 1]][0]) % prime;
                    }
                }
                if (notfound)
                {
                    proposals[0].Add(candidate1);
                    revproposals[0].Add(revcandidate1);
                }
                else
                {
                    proposals[0][j - 1] = candidate1;
                    revproposals[0][j - 1] = revcandidate1;
                }
                temp1 = gorounddown[i] * alphtopower[firstchar][twoim1];
                for (j = 0; j < proposals[0].Count; ++j)
                    AddtoDict((temp1 + proposals[0][j]) % prime, (rev_gorounddown[i - 1] + revproposals[0][j] * alphtopower[firstchar][twontwoi]) % prime, ref myhashd, ref collisions);
                temp = proposals[0];
                proposals[0] = proposals[1];
                proposals[1] = temp;
                temp = revproposals[0];
                revproposals[0] = revproposals[1];
                revproposals[1] = temp;
                //		cout << i << " " << myhashd.Count << endl;
            }
            twoim2 = (ushort)((i - 1) << 1);
            temp1 = goroundup[i] * alphtopower[firstchar][(i << 1) - 1] + alphtopower[ss1[i - 1]][twoim2] + alphtopower[ss1[i - 2]][twoim2 - 1];
            ulong temp2 = rev_goroundup[i - 1] + alphtopower[ss1[i - 2]][4] + alphtopower[ss1[i - 1]][3];
            for (j = 0; j < proposals[1].Count; ++j)
                AddtoDict((temp1 + proposals[1][j]) % prime, (temp2 + revproposals[1][j] * alphtopower[firstchar][5]) % prime, ref myhashd, ref collisions);
            AddtoDict((goroundup[i] * alphtopower[firstchar][(i << 1) - 1] + alphtopower[ss2[ncolumns - 3]][twoim2] + alphtopower[ss1[ncolumns - 2]][0] + proposals[0].Last() * alphabetsize) % prime, (alphtopower[ss1[ncolumns - 2]][twoncolumns - 1] + revproposals[0].Last() * alphtopower[firstchar][4] + alphtopower[ss2[ncolumns - 3]][3] + rev_goroundup[i - 1]) % prime, ref myhashd, ref collisions);
            temp1 = gorounddown[i] * alphtopower[firstchar][(i << 1) - 1] + alphtopower[ss2[i - 1]][twoim2] + alphtopower[ss2[i - 2]][twoim2 - 1];
            temp2 = rev_gorounddown[i - 1] + alphtopower[ss2[i - 2]][4] + alphtopower[ss2[i - 1]][3];
            for (j = 0; j < proposals[0].Count; ++j)
                AddtoDict((temp1 + proposals[0][j]) % prime, (temp2 + revproposals[0][j] * alphtopower[firstchar][5]) % prime, ref myhashd, ref collisions);
            AddtoDict((gorounddown[i] * alphtopower[firstchar][(i << 1) - 1] + alphtopower[ss1[ncolumns - 3]][twoim2] + alphtopower[ss2[ncolumns - 2]][0] + proposals[1].Last() * alphabetsize) % prime, (alphtopower[ss2[ncolumns - 2]][twoncolumns - 1] + revproposals[1].Last() * alphtopower[firstchar][4] + alphtopower[ss1[ncolumns - 3]][3] + rev_gorounddown[i - 1]) % prime, ref myhashd, ref collisions);
            return myhashd.Count + returnResult(ref collisions);
        }
        static void Main(string[] args)
        {
            byte nprovinces = byte.Parse(Console.ReadLine());
            ushort ncolumns;
            string firstrow;
            string secondrow;
            while (nprovinces-- != 0)
            {
                ncolumns = ushort.Parse(Console.ReadLine());
                firstrow = Console.ReadLine();
                secondrow = Console.ReadLine();
                Console.WriteLine(distinctwaysRK(ncolumns, ref firstrow, ref secondrow));
            }
        }
    }
}
