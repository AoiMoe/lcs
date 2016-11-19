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
#include <list>
#include "lcs.h"

using namespace std;

//
// linkage of common sequences.
//
struct PositionLink
{
	typedef int Off;
	// active node linkage to be freed.
	union {
		// previous common sequence.
		PositionLink *prev;
		// next common sequence (after reversed).
		PositionLink *next;
	} u;
	// extents of the common sequence.
	// [x0, x) / [y0, y0+x-x0)
	Off x0, x, y0;
};

class CharLCSTraits
{
public:
	typedef int Off;
	struct Fp
	{
		Off fp;
		PositionLink *pos;
		Fp(Off v=0) : fp(v), pos(NULL) { }
		void operator = (Off v)
		{
			fp = v;
		}
		operator Off () const
		{
			return fp;
		}
	};
	typedef LCS::StandardFpArray<Fp, Off> FpArray;
};

class CharLCSInstance : public LCS::StandardInstanceBase<CharLCSTraits>
{
	typedef const char *Stream;
	Stream m_a_, m_b_;
	list<PositionLink> m_pos_pool_;
public:
	PositionLink *newpos(PositionLink *prev, Off x0, Off x, Off y0)
	{
		PositionLink pos;

		pos.u.prev = prev;
		pos.x0 = x0;
		pos.x = x;
		pos.y0 = y0;

		m_pos_pool_.push_front(pos);

		return &*m_pos_pool_.begin();
	}
	void put_ses()
	{
		PositionLink *p, *next, *prev;
		Off prev_x, prev_y;

		/* reverse */
		next = NULL;
		for (p=this->fp(delta()).pos; p; p=prev) {
			prev = p->u.prev;
			p->u.next = next;
			next = p;
		}
		/* dump ses */
		prev_x = prev_y = 0;
		for (p=next; p; p=p->u.next) {
			if (prev_x < p->x0)
				printf("-%.*s\n", p->x0-prev_x, &m_a_[prev_x]);
			if (prev_y < p->y0)
				printf("+%.*s\n", p->y0-prev_y, &m_b_[prev_y]);
			if (p->x0 < p->x) {
				printf(" %.*s\n", p->x - p->x0, &m_a_[p->x0]);
				prev_x = p->x;
				prev_y = p->y0 + p->x-p->x0;
			}
		}
		if (prev_x < X())
			printf("-%.*s\n", M()-prev_x, &m_a_[prev_x]);
		if (prev_y < Y())
			printf("+%.*s\n", N()-prev_y, &m_b_[prev_y]);
	}
	// hooks
	void begin()
	{
	}
	void done()
	{
		put_ses();
	}
	bool compare(Off x, Off y)
	{
		return m_a_[x] == m_b_[y];
	}
	void common_sequence(Off x0, Off x, Off y0, Off y, Off k0, Off k)
	{
		fp(k).pos =
		    y0<y ? newpos(fp(k0).pos, x0, x, y0):fp(k0).pos;
	}
	CharLCSInstance(const char *a, const char *b)
		: LCS::StandardInstanceBase<CharLCSTraits>(
			strlen(a), strlen(b)),
		  m_a_(a), m_b_(b)
	{
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
