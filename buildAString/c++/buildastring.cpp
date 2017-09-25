// buildastring.cpp : Defines the entry point for the console application.
//

//#include "stdafx.h"
#include <iostream>
#include <string>
#include <cassert>
#include <bitset>
#include <map>
#include <vector>
#include <queue>
#include <functional>
using namespace std;
inline bool leq(unsigned short a1, unsigned short a2, unsigned short b1, unsigned short b2) // lexicographic order
{
	return(a1 < b1 || a1 == b1 && a2 <= b2);
} // for pairs
inline bool leq(unsigned short a1, unsigned short a2, unsigned short a3, unsigned short b1, unsigned short b2, unsigned short b3)
{
	return(a1 < b1 || a1 == b1 && leq(a2, a3, b2, b3));
} // and triples
  // stably sort a[0..n-1] to b[0..n-1] with keys in 0..K from r
static void radixPass(unsigned short* a, unsigned short* b, unsigned short* r, unsigned short n, size_t K)
{ // count occurrences
	unsigned short* c = new unsigned short[K + 1]; // counter array
	unsigned short i;
	for (i = 0; i <= K; i++) c[i] = 0; // reset counters
	for (i = 0; i < n; i++) c[r[a[i]]]++; // count occurrences
	unsigned short sum(0), t;
	for (i = 0; i <= K; i++) // exclusive prefix sums
	{
		t = c[i]; c[i] = sum; sum += t;
	}
	for (i = 0; i < n; i++) b[c[r[a[i]]]++] = a[i]; // sort
	delete[] c;
}
// find the suffix array SA of T[0..n-1] in {1..K}^n
// require T[n]=T[n+1]=T[n+2]=0, n>=2
void suffixArray(unsigned short* T, unsigned short* SA, unsigned short n, size_t K) {
	unsigned short n0 = (n + 2) / 3, n1 = (n + 1) / 3, n2 = n / 3, n02 = n0 + n2;
	unsigned short* R = new unsigned short[n02 + 3]; R[n02] = R[n02 + 1] = R[n02 + 2] = 0;
	unsigned short* SA12 = new unsigned short[n02 + 3]; SA12[n02] = SA12[n02 + 1] = SA12[n02 + 2] = 0;
	unsigned short* R0 = new unsigned short[n0];
	unsigned short* SA0 = new unsigned short[n0];
	//******* Step 0: Construct sample ********
	// generate positions of mod 1 and mod 2 suffixes
	// the "+(n0-n1)" adds a dummy mod 1 suffix if n%3 == 1
	unsigned short i, j(0);
	for (i = 0; i < n + (n0 - n1); i++) if (i % 3 != 0) R[j++] = i;
	//******* Step 1: Sort sample suffixes ********
	// lsb radix sort the mod 1 and mod 2 triples
	radixPass(R, SA12, T + 2, n02, K);
	radixPass(SA12, R, T + 1, n02, K);
	radixPass(R, SA12, T, n02, K);
	// find lexicographic names of triples and
	// write them to correct places in R
	unsigned short name = 0, c0 = -1, c1 = -1, c2 = -1;
	for (i = 0; i < n02; i++) {
		if (T[SA12[i]] != c0 || T[SA12[i] + 1] != c1 || T[SA12[i] + 2] != c2)
		{
			name++; c0 = T[SA12[i]]; c1 = T[SA12[i] + 1]; c2 = T[SA12[i] + 2];
		}
		if (SA12[i] % 3 == 1) { R[SA12[i] / 3] = name; } // write to R1
		else { R[SA12[i] / 3 + n0] = name; } // write to R2
	}
	// recurse if names are not yet unique
	if (name < n02) {
		suffixArray(R, SA12, n02, name);
		// store unique names in R using the suffix array
		for (i = 0; i < n02; i++) R[SA12[i]] = i + 1;
	}
	else // generate the suffix array of R directly
		for (i = 0; i < n02; i++) SA12[R[i] - 1] = i;
	//******* Step 2: Sort nonsample suffixes ********
	// stably sort the mod 0 suffixes from SA12 by their first character
	for (i = 0, j = 0; i < n02; i++) if (SA12[i] < n0) R0[j++] = 3 * SA12[i];
	radixPass(R0, SA0, T, n0, K);
	//******* Step 3: Merge ********
	// merge sorted SA0 suffixes and sorted SA12 suffixes
	for (unsigned short p = 0, t = n0 - n1, k = 0; k < n; k++) {
#define GetI() (SA12[t] < n0 ? SA12[t] * 3 + 1 : (SA12[t] - n0) * 3 + 2)
		i = GetI(); // pos of current offset 12 suffix
		j = SA0[p]; // pos of current offset 0 suffix
		if (SA12[t] < n0 ? // different compares for mod 1 and mod 2 suffixes
			leq(T[i], R[SA12[t] + n0], T[j], R[j / 3]) :
			leq(T[i], T[i + 1], R[SA12[t] - n0 + 1], T[j], T[j + 1], R[j / 3 + n0]))
		{ // suffix from SA12 is smaller
			SA[k] = i; t++;
			if (t == n02) // done --- only SA0 suffixes left
				for (k++; p < n0; p++, k++) SA[k] = SA0[p];
		}
		else { // suffix from SA0 is smaller
			SA[k] = j; p++;
			if (p == n0) // done --- only SA12 suffixes left
				for (k++; t < n02; t++, k++) SA[k] = GetI();
		}
	}
	delete[] R; delete[] SA12; delete[] SA0; delete[] R0;
}
void findsuffixarray(unsigned short n, const string &s, unsigned short *SA)
{
	unsigned short i;
	map<unsigned char, vector<unsigned short>>mymap;
	pair<map<unsigned char, vector<unsigned short>>::iterator, bool>insertres;
	vector<unsigned short>myv(1);
	for (i = 0; i < n; ++i)
	{
		myv[0] = i;
		insertres = mymap.insert(make_pair(s[i], myv));
		if (!insertres.second)insertres.first->second.push_back(i);
	}
	map<unsigned char, vector<unsigned short>>::iterator itr = mymap.begin();
	size_t K;
	unsigned short counter(1);
	unsigned short *T = new unsigned short[n + 3];
	while (itr != mymap.end())
	{
		K = itr->second.size();
		for (i = 0; i < K; ++i)T[itr->second[i]] = counter;
		++counter;
		++itr;
	}
	K = mymap.size();
	T[n] = T[n + 1] = T[n + 2] = 0;
	suffixArray(T, SA, n, K);
	delete[] T;
}

//adaptation of  kassai algorithm for finding the LCP array
void kasai(const unsigned short *SA, const string &s, unsigned short n, unsigned short *mylcp, unsigned short minval)
{
	// Process all suffixes one by one starting from
	// first suffix in txt[]
	unsigned short i, j, k(0);
	unsigned short *invSA = new unsigned short[n];
	unsigned short *lcp = new unsigned short[n];
	for (i = 0; i < n; ++i)
	{
		invSA[SA[i]] = i;
		lcp[i] = 0;
	}
	for (i = 0; i<n; ++i)
	{
		/* If the current suffix is at n-1, then we don�t
		have next substring to consider. So lcp is not
		defined for this substring, we put zero. */
		if (invSA[i] == n - 1)
		{
			k = 0;
			continue;
		}

		/* j contains index of the next substring to
		be considered  to compare with the present
		substring, i.e., next string in suffix array */
		j = SA[invSA[i] + 1];

		// Directly start matching from k'th index as
		// at-least k-1 characters will match
		while (i + k<n && j + k<n && s[i + k] == s[j + k]) ++k;
		if (k > minval)	lcp[invSA[i]] = k;
		if (k>0)--k;
	}
	vector<vector<unsigned short>>buckets;
	bool prevlcpisnull = true;
	unsigned short ii, jj;
	for (i = 0; i < n; ++i)
	{
		if (lcp[i] != 0)
		{
			if (!prevlcpisnull)	buckets.back().push_back(SA[i]);
			else
			{
				buckets.push_back({ SA[i] });
				prevlcpisnull = false;
			}
		}
		else prevlcpisnull = true;
	}
	unsigned short nbuckets = buckets.size();
	unsigned short minlcp;
	unsigned short howmanyineachbucket;
	unsigned short candidateval;
	for (i = 0; i < nbuckets; ++i)
	{
		howmanyineachbucket = buckets[i].size();
		for (j = 0; j < howmanyineachbucket; ++j)
		{
			ii = buckets[i][j];
			jj = SA[invSA[ii] + 1];
			minlcp = lcp[invSA[ii]];
			if (jj < ii)
			{
				candidateval = ii - jj<minlcp ? ii - jj : minlcp;
				if (mylcp[ii] == 0 || mylcp[ii]<candidateval)mylcp[ii] = candidateval;
			}
			else if (ii < jj)
			{
				candidateval = jj - ii<minlcp ? jj - ii : minlcp;
				if (mylcp[jj] == 0 || mylcp[jj]<candidateval)mylcp[jj] = candidateval;
			}
			for (k = j + 1; k < howmanyineachbucket; ++k)
			{
				if (lcp[invSA[jj]] < minlcp)minlcp = lcp[invSA[jj]];
				jj = SA[invSA[jj] + 1];
				if (jj < ii)
				{
					candidateval = ii - jj<minlcp ? ii - jj : minlcp;
					if (mylcp[ii] == 0 || mylcp[ii]<candidateval)mylcp[ii] = candidateval;
				}
				else if (ii < jj)
				{
					candidateval = jj - ii<minlcp ? jj - ii : minlcp;
					if (mylcp[jj] == 0 || mylcp[jj]<candidateval)mylcp[jj] = candidateval;
				}
			}
		}
	}
	delete[] invSA;
}
//
unsigned int mincost(const string &s, unsigned short n, unsigned short charcost, unsigned short substrcost)
{
	unsigned short *SA = new unsigned short[n];
	findsuffixarray(n, s, SA);
	unsigned short *invSA = new unsigned short[n];
	unsigned short *mylcp = new unsigned short[n];
	unsigned short i;
	for (i = 0; i < n; ++i) mylcp[i] = 0;
	unsigned short minval;
	if (substrcost%charcost == 0)minval = substrcost / charcost;
	else if (substrcost < charcost)minval = 0;
	else minval = substrcost / charcost - 1;
	//Calculates LCP array containing substrings that match the required string
	kasai(SA, s, n, mylcp, minval);
	delete[] SA;
	unsigned int returncost(charcost);
	i = 1;
	while (i<n && mylcp[i] <= minval)
	{
		if (mylcp[i] == 0)returncost += charcost;
		else returncost += charcost<substrcost ? charcost : substrcost;
		++i;
	}

	//application of Dykstra algorithm to find the minimum cost substring additions among the substrings picked by the LCP array
	if (i == n)return returncost;
	unsigned short savedi = i;
	unsigned int maxcost = n*(charcost>substrcost ? charcost : substrcost);
	vector<unsigned int>costateachnode(n - savedi, maxcost);
	costateachnode[0] = returncost + charcost;
	priority_queue<pair<unsigned int, unsigned short>, vector<pair<unsigned int, unsigned short>>, greater<pair<unsigned int, unsigned short>>> mypq;
	mypq.push(make_pair(returncost + charcost, savedi));
	if (mylcp[savedi] > 1)
	{
		costateachnode[mylcp[savedi] - 1] = returncost + substrcost;
		mypq.push(make_pair(returncost + substrcost, savedi + mylcp[savedi] - 1));
	}
	else if (mylcp[savedi] == 1 && substrcost < charcost)
	{
		costateachnode[0] = returncost + substrcost;
		mypq.push(make_pair(costateachnode[0], savedi));
	}
	while (!mypq.empty())
	{
		i = mypq.top().second;
		returncost = costateachnode[i - savedi];
		if (++i == n)break;
		mypq.pop();
		if (costateachnode[i - savedi] > returncost + charcost)
		{
			costateachnode[i - savedi] = returncost + charcost;
			mypq.push(make_pair(returncost + charcost, i));
		}
		if (mylcp[i] != 0 && costateachnode[i + mylcp[i] - 1 - savedi] > returncost + substrcost)
		{
			costateachnode[i + mylcp[i] - 1 - savedi] = returncost + substrcost;
			mypq.push(make_pair(returncost + substrcost, i + mylcp[i] - 1));
		}
	}
	delete[] mylcp;
	return returncost;
}
int main()
{
	unsigned short t, n, charcost, substrcost;
	cin >> t;
	assert(t > 0 && t < 4);
	string s;
	while (t-- > 0)
	{
		cin >> n;
		assert(n > 0 && n < 30001);
		cin >> charcost;
		assert(charcost > 0 && charcost < 10001);
		cin >> substrcost;
		assert(substrcost > 0 && substrcost < 10001);
		cin >> s;
		assert(s.length() == n);
		cout << mincost(s, n, charcost, substrcost) << endl;
	}
	return 0;
}
