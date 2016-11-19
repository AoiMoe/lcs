/*-
 * Copyright (c)2005 Takuya SHIOZAKI,
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include "lcs.h"

using namespace std;

class CharLCSTraits
{
public:
	typedef int Off;
	typedef Off Fp;
	typedef LCS::StandardFpArray<Fp, Off> FpArray;
};

class CharLCSInstance : public LCS::StandardInstanceBase<CharLCSTraits>
{
	typedef const char *Stream;
	Stream m_a_, m_b_;
public:
	CharLCSInstance(const char *a, const char *b)
		: LCS::StandardInstanceBase<CharLCSTraits>(
			strlen(a), strlen(b)),
		  m_a_(a), m_b_(b)
	{
	}
	void common_sequence(Off x0, Off x, Off y0, Off y, Off k0, Off k)
	{
		printf("  fp[%d] -> fp[%d]", k0, k);
		if (y0 < y) {
			printf(", snake [%d,%d):[%d,%d) \"%.*s\":\"%.*s\"",
			       x0, x, y0, y,
			       x-x0, &m_a_[x0], y-y0, &m_b_[y0]);
		}
		printf("\n");
	}
	bool compare(Off x, Off y) const
	{
		return m_a_[x] == m_b_[y];
	}
	void begin()
	{
		printf("M=%d, N=%d, delta=%d\n", M(), N(), delta());
	}
	void done()
	{
		printf("p=%d, D=%d\n", p(), D());
	}
};

int
main(int argc, char **argv)
{
	if (argc != 3) {
		fprintf(stderr, "usage: %s <str1> <str2>\n", argv[0]);
		return 1;
	}

	CharLCSInstance intr(argv[1], argv[2]);
	LCS::calculate(intr);

	return 0;
}
