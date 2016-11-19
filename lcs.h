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

#ifndef LCS_H_
#define LCS_H_

//
// Calculate Largest Common Sequence (LCS).
//
// This template class implements the algorithm [1].
//
//   [1] S. Wu, E. Myers, U. Manber, and W. Miller,
//       "An O(NP) Sequence Comparison Algorithm,"
//       Information Processing Letters 35, 6 (1990), 317-323
//       http://www.eecs.berkeley.edu/~gene/Papers/np_diff.pdf
//

namespace LCS
{

#define LCS_FINAL_		/* the member fn unable to be overriden */
#define LCS_NEED_OVERRIDE_	/* the member fn needed to be overriden */

//
// class Detail_ - implementation details of the algorithm.
//
// class Inst_ must have these types/members as public:
//	typename Traits_::Off Off;
//	typename Traits_::Fp Fp;
//	typedef typename Traits_::FpArray FpArray;
//	Off M() const;
//	Off N() const;
//	Off delta() const;
//	Off p() const;
//	Fp &fp(Off k);
//	void incr_p();
//	bool isswap() const;
//	bool compare(Off x, Off y) const;
//	void common_sequence(Off x0, Off x, Off y0, Off y, Off k0, Off k);
//	void begin();
//	void done();
//
template <class Inst_>
class Detail_
{
	friend void calculate<>(Inst_ &);
private:
	Detail_(); // cannot create any instance of this class.
	// types
	typedef typename Inst_::Off Off_;
	typedef typename Inst_::Fp Fp_;
	typedef typename Inst_::FpArray FpArray_;

	typedef Off_ Swapper_(Off_, Off_);
	static Off_ first_(Off_ x, Off_ y)
	{
		return x;
	}
	static Off_ second_(Off_ x, Off_ y)
	{
		return y;
	}

	// calculate fp[k] from either fp[k-1] or fp[k+1]
	template <Swapper_ Fx_, Swapper_ Fy_>
	static Off_ calculate_fp_k_(Inst_ &inst, Off_ k)
	{
		Off_ vs_y0, hs_y0, vs_k0, hs_k0, x0, y0, k0, x, y;

		// choose either fp[k-1] or fp[k+1] for seed
		vs_k0 = k-1;
		vs_y0 = inst.fp(vs_k0) + 1;
		hs_k0 = k+1;
		hs_y0 = inst.fp(hs_k0);

		if (vs_y0 >= hs_y0) {
			y = y0 = vs_y0;
			k0 = vs_k0;
		} else {
			y = y0 = hs_y0;
			k0 = hs_k0;
		}
		x0 = x = y-k;

		// snake
		while (x<inst.M() && y<inst.N() &&
		       inst.compare(Fx_(x, y), Fy_(x, y)))
			x++, y++;

		inst.common_sequence(Fx_(x0, y0), Fx_(x, y),
				     Fy_(x0, y0), Fy_(x, y),
				     k0, k);

		return y;
	}
	/* main body of the calculation of LCS */
	template <Swapper_ Fx_, Swapper_ Fy_>
	static void calculate_main_tmpl_(Inst_ &inst)
	{
		inst.begin();

		Fp_ &fp_delta = inst.fp(inst.delta());

		do {
			Off_ k;
			inst.incr_p();
			for (k=-inst.p(); k<inst.delta(); ++k)
				inst.fp(k) =
				    calculate_fp_k_<Fx_, Fy_>(inst, k);
			for (k=inst.delta()+inst.p(); k>=inst.delta(); --k)
				inst.fp(k) =
				    calculate_fp_k_<Fx_, Fy_>(inst, k);
		} while (fp_delta != inst.N());

		inst.done();
	}
	static void calculate_main_(Inst_ &inst)
	{
		calculate_main_tmpl_<first_, second_>(inst);
	}
	static void calculate_main_swap_(Inst_ &inst)
	{
		calculate_main_tmpl_<second_, first_>(inst);
	}
	static void calculate_(Inst_ &inst)
	{
		if (inst.isswap())
			calculate_main_swap_(inst);
		else
			calculate_main_(inst);
	}
};

// public function to calcurate LCS
template <class Inst_>
void calculate(Inst_ &inst)
{
	Detail_<Inst_>::calculate_(inst);
}

// public function to calcurate D (enabled after LCS calculated on the 'inst')
template <class Inst_>
typename Inst_::Off D(Inst_ &inst)
{
	return inst.delta()+inst.p()*2;
}


//
// Standard instance base class for the algorithm.
//
template <class Traits_>
class StandardInstanceBase
{
public:
	typedef typename Traits_::Off Off;
	typedef typename Traits_::Fp Fp;
	typedef typename Traits_::FpArray FpArray;
private:
	typedef Detail_<StandardInstanceBase<Traits_> > Detail_;
	bool m_isswap_;
	Off m_M_, m_N_, m_delta_, m_p_;
	FpArray m_fp_array_;
public:
	// for class Detail_:
	LCS_FINAL_ Off M() const { return m_M_; }
	LCS_FINAL_ Off N() const { return m_N_; }
	LCS_FINAL_ Off delta() const { return m_delta_; }
	LCS_FINAL_ Off p() const { return m_p_; }
	LCS_FINAL_ Fp &fp(Off k)
	{
		return m_fp_array_[k];
	}
	LCS_FINAL_ void incr_p() { m_p_++; }
	LCS_FINAL_ bool isswap() const { return m_isswap_; }
	LCS_NEED_OVERRIDE_ bool compare(Off, Off) const;
	void common_sequence(Off, Off, Off, Off, Off, Off) { }
	void begin() { }
	void done() { }
	// convenience routines:
	LCS_FINAL_ Off X() const { return m_isswap_ ? m_N_:m_M_; }
	LCS_FINAL_ Off Y() const { return m_isswap_ ? m_M_:m_N_; }
	LCS_FINAL_ Off D() const { return LCS::D(*this); }
protected:
	StandardInstanceBase(Off a_X, Off a_Y)
		: m_isswap_(a_X>a_Y),
		  m_M_(m_isswap_ ? a_Y:a_X),
		  m_N_(m_isswap_ ? a_X:a_Y),
		  m_delta_(m_N_-m_M_),
		  m_p_(-1),
		  m_fp_array_(-(m_M_+1), (m_N_+1), -1)
	{
	}
	~StandardInstanceBase()
	{
	}
};

//
// Simple array template for LCS.
//
// FpArray must be able to accept -(M+1)..(N+1) as its index.
//
template <typename Fp_, typename Off_>
class StandardFpArray
{
	Fp_ *m_fp_head_, *m_fp_;
public:
	StandardFpArray(Off_ min, Off_ max, Off_ ival)
	{
		m_fp_head_ = new Fp_[max-min+1];
		m_fp_ = m_fp_head_ - min;
		for (Off_ k=min; k<=max; k++)
			m_fp_[k] = ival;
	}
	~StandardFpArray()
	{
		delete m_fp_head_;
	}
	Fp_ &operator [] (Off_ k)
	{
		return m_fp_[k];
	}
};

} // namespace LCS

#endif
