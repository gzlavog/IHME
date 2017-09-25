// gridLandProvinces.cpp : Defines the entry point for the console application.
//

//#include "stdafx.h"
#include <iostream>
#include <vector>
#include <string>
#include <unordered_set>
#include <bitset>
#include <unordered_map>
#include <climits>
using namespace std;

const unsigned long long prime = 999990099413;
//builds the alphabet composed of the letters found in strings s1 and s2
//adds to rolling hash dictionary with fixed length words starting at each possible position
unsigned char getalphtopower(const char *s1, const char *s2, unsigned short ncolumns, unsigned short twoncolumns,
	unordered_map<char, unsigned long long *> &alphtopower, char &firstchar)
{
	bitset<26> mybitset;
	unsigned short i;
	for (i = 0; i < ncolumns; ++i)
	{
		mybitset.set(s1[i] - 'a');
		mybitset.set(s2[i] - 'a');
	}
	vector<char> vofchars;
	for (i = 0; i < 26; ++i)if (mybitset.test(i))vofchars.push_back('a' + i);
	unsigned char alphabetsize = vofchars.size();
	firstchar = vofchars[0];
	if (alphabetsize == 1)return 1;
	if (ncolumns == 1) return 2;
	alphtopower[firstchar] = new unsigned long long[twoncolumns];
	alphtopower[firstchar][0] = 1;
	alphtopower[firstchar][1] = alphabetsize;
	//rolling hash build
	for (i = 2; i < twoncolumns; ++i) alphtopower[firstchar][i] = (alphtopower[firstchar][i - 1] * alphabetsize) % prime;
	for (unsigned char j = 1; j < alphabetsize; ++j)
	{
		alphtopower[vofchars[j]] = new unsigned long long[twoncolumns];
		//rolling hash build
		for (i = 0; i < twoncolumns; ++i) alphtopower[vofchars[j]][i] = (alphtopower[vofchars[j - 1]][i] + alphtopower[firstchar][i]) % prime;
	}
	return alphabetsize;
}
void RabinKarpArray(unordered_map<char, unsigned long long *> &alphtopower, unsigned short ncolumns, const char *s1, const char *s2,
	unsigned long long *hasharray1, unsigned long long *revhasharray1, unsigned long long *hasharray2, unsigned long long *revhasharray2, unsigned char alphabetsize)
{
	unsigned short ncolm1(ncolumns - 1);
	hasharray1[ncolm1] = alphtopower[s1[ncolm1]][2] + alphtopower[s2[ncolm1]][1] + alphtopower[s2[ncolumns - 2]][0];
	hasharray2[ncolm1] = alphtopower[s2[ncolm1]][2] + alphtopower[s1[ncolm1]][1] + alphtopower[s1[ncolumns - 2]][0];
	revhasharray1[ncolm1 - 1] = alphtopower[s1[ncolm1]][0] + alphtopower[s2[ncolm1]][1] + alphtopower[s2[ncolumns - 2]][2];
	revhasharray2[ncolm1 - 1] = alphtopower[s2[ncolm1]][0] + alphtopower[s1[ncolm1]][1] + alphtopower[s1[ncolumns - 2]][2];
	for (unsigned short i = ncolm1 - 1; i != 0; --i)
	{
		hasharray1[i] = (hasharray1[i + 1] * alphabetsize + alphtopower[s1[i]][(ncolumns - i) << 1] + alphtopower[s2[i - 1]][0]) % prime;
		hasharray2[i] = (hasharray2[i + 1] * alphabetsize + alphtopower[s2[i]][(ncolumns - i) << 1] + alphtopower[s1[i - 1]][0]) % prime;
		revhasharray1[i - 1] = (revhasharray1[i] * alphabetsize + alphtopower[s1[i]][0] + alphtopower[s2[i - 1]][(ncolumns - i) << 1]) % prime;
		revhasharray2[i - 1] = (revhasharray2[i] * alphabetsize + alphtopower[s2[i]][0] + alphtopower[s1[i - 1]][(ncolumns - i) << 1]) % prime;
	}
	hasharray1[0] = (hasharray1[1] + alphtopower[s1[0]][(ncolumns << 1) - 1]) % prime;
	hasharray2[0] = (hasharray2[1] + alphtopower[s2[0]][(ncolumns << 1) - 1]) % prime;
}

void getlastnodehashes(unordered_map<char, unsigned long long *> &alphtopower, unsigned short ncolumns, const char *s1, const char *s2, unordered_set<unsigned long long> &myhashd)
{
	unsigned short ncolm1 = ncolumns - 1;
	unsigned long long result = alphtopower[s2[ncolm1]][0];
	int i;
	for (i = ncolm1 - 1; i != -1; --i)result += alphtopower[s2[i]][ncolm1 - i];
	result %= prime;
	for (i = 0; i < ncolumns; ++i)result += alphtopower[s1[i]][ncolumns + i];
	if (myhashd.insert(result%prime).second)
	{
		result = alphtopower[s1[ncolm1]][0];
		for (i = ncolm1 - 1; i != -1; --i)result += alphtopower[s1[i]][ncolm1 - i];
		result %= prime;
		for (i = 0; i < ncolumns; ++i)result += alphtopower[s2[i]][ncolumns + i];
		myhashd.insert(result%prime);
	}
}
unsigned long long multiply(unsigned long long a, unsigned char alphabetsize, unsigned short dim, const unsigned long long &upto)
{
	while (dim-- > 0)
	{
		if (a < upto) a *= alphabetsize;
		else a = (a%prime)*alphabetsize;
	}
	return a;
}
//gooes through all possible ways of building the string and makes a dictionary of hashes for all strings built, calculating an int hashvalue for each string.
// uses Rabin-Karp algorithm to build the string hash

size_t distinctwaysRK(unsigned short ncolumns, const char *ss1, const char *ss2)
{
	unordered_map<char, unsigned long long *> alphtopower;
	unsigned short twoncolumns = ncolumns << 1;
	char firstchar;
	unsigned char alphabetsize = getalphtopower(ss1, ss2, ncolumns, twoncolumns, alphtopower, firstchar);
	if (alphabetsize == 1)return 1;
	if (ncolumns == 1) return 2;
	unsigned long long *goroundup = new unsigned long long[ncolumns];
	unsigned short ncolsm1(ncolumns - 1);
	unsigned long long *rev_goroundup = new unsigned long long[ncolsm1];
	unsigned long long *gorounddown = new unsigned long long[ncolumns];
	unsigned long long *rev_gorounddown = new unsigned long long[ncolsm1];
	//there are multiple ways to build the final string: 1.going right in the first string (goroundup) + snaking up-down till touching all remaining nodes)
	// or                                                2. like in but 1 in reverse
	RabinKarpArray(alphtopower, ncolumns, ss1, ss2, goroundup, rev_goroundup, gorounddown, rev_gorounddown, alphabetsize);
	//Start at Node 0;
	unordered_set<unsigned long long> myhashd = { goroundup[0] };
	myhashd.insert(gorounddown[0]);
	//Node 1
	if (myhashd.insert((goroundup[1] * alphabetsize + alphtopower[ss1[0]][0]) % prime).second)
		myhashd.insert((rev_goroundup[0] + alphtopower[ss1[0]][twoncolumns - 1]) % prime);
	if (myhashd.insert((gorounddown[1] * alphabetsize + alphtopower[ss2[0]][0]) % prime).second)
		myhashd.insert((rev_gorounddown[0] + alphtopower[ss2[0]][twoncolumns - 1]) % prime);
	//Lastnode
	getlastnodehashes(alphtopower, ncolumns, ss1, ss2, myhashd);
	//Node 2 up
	if (ncolumns == 2) return myhashd.size();
	vector<unsigned long long>proposals[2] = { { alphtopower[ss1[1]][2] + alphtopower[ss1[0]][1] + alphtopower[ss2[0]][0] },
	{ alphtopower[ss2[1]][2] + alphtopower[ss2[0]][1] + alphtopower[ss1[0]][0] } };
	vector<unsigned long long>revproposals[2] = { { alphtopower[ss2[0]][2] + alphtopower[ss1[0]][1] + alphtopower[ss1[1]][0] },
	{ alphtopower[ss1[0]][2] + alphtopower[ss2[0]][1] + alphtopower[ss2[1]][0] } };
	unsigned long long upto = ULLONG_MAX / alphabetsize;
	unsigned long long temp1(multiply(goroundup[2], alphabetsize, 3, upto));
	if (myhashd.insert((temp1 + proposals[0][0]) % prime).second)
		myhashd.insert((rev_goroundup[1] + multiply(revproposals[0][0], alphabetsize, twoncolumns - 3, upto)) % prime);
	if (ss2[0] != ss1[1])
	{
		proposals[0].push_back(revproposals[0][0]);
		revproposals[0].push_back(proposals[0][0]);
		if (myhashd.insert((temp1 + proposals[0][1]) % prime).second)
			myhashd.insert((rev_goroundup[1] + multiply(revproposals[0][1], alphabetsize, twoncolumns - 3, upto)) % prime);
	}
	//Node 2 down
	temp1 = multiply(gorounddown[2], alphabetsize, 3, upto);
	if (myhashd.insert((temp1 + proposals[1][0]) % prime).second)
		myhashd.insert((rev_gorounddown[1] + multiply(revproposals[1][0], alphabetsize, twoncolumns - 3, upto)) % prime);
	if (ss1[0] != ss2[1])
	{
		proposals[1].push_back(revproposals[1][0]);
		revproposals[1].push_back(proposals[1][0]);
		if (myhashd.insert((temp1 + proposals[1][1]) % prime).second)
			myhashd.insert((rev_gorounddown[1] + multiply(revproposals[1][1], alphabetsize, twoncolumns - 3, upto)) % prime);
	}
	//Node 3 and up
	//		cout << myhashd.size() << endl;
	if (ncolumns == 3) return myhashd.size();
	unsigned short i, j, twoim2, twoim1, twontwoi;
	bool notfound;
	unsigned long long candidate0, candidate1, revcandidate1, temp2;
	for (i = 3; i < ncolsm1; ++i)
	{
		twoim2 = (i - 1) << 1;
		twoim1 = twoim2 + 1;
		candidate1 = (proposals[1].back()*alphabetsize + alphtopower[ss1[i - 2]][twoim2] + alphtopower[ss2[i - 1]][0]) % prime;
		revcandidate1 = (revproposals[1].back()*alphabetsize + alphtopower[ss2[i - 1]][twoim2] + alphtopower[ss1[i - 2]][0]) % prime;
		candidate0 = (proposals[0].back()*alphabetsize + alphtopower[ss2[i - 2]][twoim2] + alphtopower[ss1[i - 1]][0]) % prime;
		notfound = true;
		temp1 = alphtopower[ss1[i - 1]][twoim2] + alphtopower[ss1[i - 2]][twoim2 - 1];
		temp2 = alphtopower[ss1[i - 2]][1] + alphtopower[ss1[i - 1]][0];
		for (j = 0; j < proposals[1].size(); ++j)
		{
			if (notfound)
			{
				proposals[1][j] = (proposals[1][j] + temp1) % prime;
				if (proposals[1][j] == candidate0) notfound = false;
				revproposals[1][j] = (revproposals[1][j] * alphtopower[firstchar][2] + temp2) % prime;
				//				else revproposals[1][j] = (multiply(revproposals[1][j], alphabetsize, 2, upto) + temp2) % prime;
			}
			else
			{
				proposals[1][j - 1] = (proposals[1][j] + temp1) % prime;
				revproposals[1][j - 1] = (multiply(revproposals[1][j], alphabetsize, 2, upto) + temp2) % prime;
			}
		}
		if (notfound)
		{
			proposals[1].push_back(candidate0);
			revproposals[1].push_back((revproposals[0].back()*alphabetsize + alphtopower[ss1[i - 1]][twoim2] + alphtopower[ss2[i - 2]][0]) % prime);
		}
		else
		{
			proposals[1][j - 1] = candidate0;
			revproposals[1][j - 1] = (revproposals[0].back()*alphabetsize + alphtopower[ss1[i - 1]][twoim2] + alphtopower[ss2[i - 2]][0]) % prime;
		}
		temp1 = multiply(goroundup[i], alphabetsize, twoim1, upto);
		twontwoi = twoncolumns - twoim1;
		for (j = 0; j < proposals[1].size(); ++j)
		{
			if (myhashd.insert((temp1 + proposals[1][j]) % prime).second)
				//				myhashd.insert((rev_goroundup[i - 1] + (revproposals[1][j]*alphtopower[firstchar][twontwoi]) % prime) % prime);
				myhashd.insert((rev_goroundup[i - 1] + multiply(revproposals[1][j], alphabetsize, twontwoi, upto)) % prime);
		}
		notfound = true;
		temp1 = alphtopower[ss2[i - 1]][twoim2] + alphtopower[ss2[i - 2]][twoim2 - 1];
		temp2 = alphtopower[ss2[i - 2]][1] + alphtopower[ss2[i - 1]][0];
		for (j = 0; j < proposals[0].size(); ++j)
		{
			if (notfound)
			{
				proposals[0][j] = (proposals[0][j] + temp1) % prime;
				if (proposals[0][j] == candidate1) notfound = false;
				else revproposals[0][j] = (revproposals[0][j] * alphtopower[firstchar][2] + temp2) % prime;
			}
			else
			{
				proposals[0][j - 1] = (proposals[0][j] + temp1) % prime;
				revproposals[0][j - 1] = (multiply(revproposals[0][j], alphabetsize, 2, upto) + temp2) % prime;
			}
		}
		if (notfound)
		{
			proposals[0].push_back(candidate1);
			revproposals[0].push_back(revcandidate1);
		}
		else
		{
			proposals[0][j - 1] = candidate1;
			revproposals[0][j - 1] = revcandidate1;
		}
		temp1 = multiply(gorounddown[i], alphabetsize, twoim1, upto);
		for (j = 0; j < proposals[0].size(); ++j)
		{
			if (myhashd.insert((temp1 + proposals[0][j]) % prime).second)
				myhashd.insert((rev_gorounddown[i - 1] + multiply(revproposals[0][j], alphabetsize, twontwoi, upto)) % prime);
		}
		proposals[0].swap(proposals[1]);
		revproposals[0].swap(revproposals[1]);
		//		cout << i << " " << myhashd.size() << endl;
	}
	twoim2 = (i - 1) << 1;
	temp1 = multiply(goroundup[i], alphabetsize, (i << 1) - 1, upto) + alphtopower[ss1[i - 1]][twoim2] + alphtopower[ss1[i - 2]][twoim2 - 1];
	temp2 = rev_goroundup[i - 1] + alphtopower[ss1[i - 2]][4] + alphtopower[ss1[i - 1]][3];
	for (j = 0; j < proposals[1].size(); ++j)
	{
		if (myhashd.insert((temp1 + proposals[1][j]) % prime).second)
			myhashd.insert((temp2 + multiply(revproposals[1][j], alphabetsize, 5, upto)) % prime);
	}
	if (myhashd.insert((multiply(goroundup[i], alphabetsize, (i << 1) - 1, upto) + alphtopower[ss2[ncolumns - 3]][twoim2] + alphtopower[ss1[ncolumns - 2]][0] +
		proposals[0].back()*alphabetsize) % prime).second)
		myhashd.insert((alphtopower[ss1[ncolumns - 2]][twoncolumns - 1] + multiply(revproposals[0].back(), alphabetsize, 4, upto) +
			alphtopower[ss2[ncolumns - 3]][3] + rev_goroundup[i - 1]) % prime);
	delete[] goroundup;
	delete[] rev_goroundup;
	temp1 = (multiply(gorounddown[i], alphabetsize, (i << 1) - 1, upto) + alphtopower[ss2[i - 1]][twoim2] + alphtopower[ss2[i - 2]][twoim2 - 1]) % prime;
	temp2 = (rev_gorounddown[i - 1] + alphtopower[ss2[i - 2]][4] + alphtopower[ss2[i - 1]][3]) % prime;
	for (j = 0; j < proposals[0].size(); ++j)
	{
		if (myhashd.insert((temp1 + proposals[0][j]) % prime).second)
			myhashd.insert((temp2 + multiply(revproposals[0][j], alphabetsize, 5, upto)) % prime);
	}
	if (myhashd.insert((multiply(gorounddown[i], alphabetsize, (i << 1) - 1, upto) + alphtopower[ss1[ncolumns - 3]][twoim2] + alphtopower[ss2[ncolumns - 2]][0] +
		proposals[1].back()*alphabetsize) % prime).second)
		myhashd.insert((alphtopower[ss2[ncolumns - 2]][twoncolumns - 1] + multiply(revproposals[1].back(), alphabetsize, 4, upto) +
			alphtopower[ss1[ncolumns - 3]][3] + rev_gorounddown[i - 1]) % prime);
	delete[] gorounddown;
	delete[] rev_gorounddown;
	return myhashd.size();
}

int main()
{
	short nprovinces;
	cin >> nprovinces;
	//	assert(nprovinces != 0 && nprovinces < 16);
	unsigned short ncolumns;
	char *firstrow;
	char *secondrow;
	while (nprovinces-- != 0)
	{
		cin >> ncolumns;
		//		assert(ncolumns != 0 && ncolumns < 601);
		firstrow = new char[ncolumns + 1];
		cin >> firstrow;
		//		assert(firstrow.size() == ncolumns);
		secondrow = new char[ncolumns + 1];
		cin >> secondrow;
		//		assert(secondrow.size() == ncolumns);
		//		cout<<distinctways(ncolumns, firstrow, secondrow)<<endl;
		cout << distinctwaysRK(ncolumns, firstrow, secondrow) << endl;
		delete[] firstrow;
		delete[] secondrow;
	};
	return 0;
}
