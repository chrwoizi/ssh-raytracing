#ifndef SIMD_SSE_H
#define SIMD_SSE_H

#ifndef SIMD_INTERNAL
#error dont include simd_sse.h directly, use simd.h instead !
#endif

#include <cassert>

#ifndef WINDOWS
#include <mm_malloc.h>
#endif

#include <xmmintrin.h>


#if 0
// stack alignment hack - geborgt von http://www.fftw.org
#define WITH_ALIGNED_STACK(what)                                \
{                                                               \
     /*                                                         \
      * Use alloca to allocate some memory on the stack.        \
      * This alerts gcc that something funny is going           \
      * on, so that it does not omit the frame pointer          \
      * etc.                                                    \
      */                                                        \
     (void)__builtin_alloca(16);                                \
                                                                \
     /*                                                         \
      * Now align the stack pointer                             \
      */                                                        \
     __asm__ __volatile__ ("andl $-16, %esp");                  \
                                                                \
     what                                                       \
}
#endif

/* IDEAS
 *
 *	o add (compiler specific) inline enforcment ?
 *		gcc: __attribute__((__always_inline__))
 *		icc: ?
 */

/*
 * provides new and delete operators to ensure alignment for dynamic memory
 * EVERY CLASS WITH SIMD TYPE MEMBERS HAS TO INHERIT FROM SIMDmemAligned !!!
 */
#if 0
template <int alignment> class memAligned
{
  public:
	memAligned() { assert(reinterpret_cast<unsigned int>(this) % alignment == 0); }
	
	void *operator new(size_t size)	{ return _mm_malloc(size, alignment); }
	
	void operator delete (void *mem) { _mm_free(mem); }
};
#else
template <int alignment> class memAligned
{
  public:
	memAligned() { assert(reinterpret_cast<unsigned long>(this) % alignment == 0); }
	
	void *operator new(size_t size)	{
		// Check if alignment is >= 4 and a power of 2
		assert((alignment >= 4) && ((alignment & (alignment - 1)) == 0));
		// Allocate memory
		void* data = ::operator new(size + alignment + sizeof(void*));
		// Calculate address of aligned memory block
		unsigned long address = reinterpret_cast<unsigned long>(data);
		address = (address + sizeof(void*) + alignment - 1) & ~(alignment - 1);
		// Store address of unaligned memory block
		void** ptr = reinterpret_cast<void**>(address - sizeof(void*));
		*ptr = data;
		// Return pointer to aligned memory block
		return reinterpret_cast<void*>(address);
	}
	
	void *operator new[](size_t size)	{
		// Check if alignment is >= 4 and a power of 2
		assert((alignment >= 4) && ((alignment & (alignment - 1)) == 0));
		// Allocate memory
		void* data = ::operator new(size + alignment + sizeof(void*));
		// Calculate address of aligned memory block
		unsigned long address = reinterpret_cast<unsigned long>(data);
		address = (address + sizeof(void*) + alignment - 1) & ~(alignment - 1);
		// Store address of unaligned memory block
		void** ptr = reinterpret_cast<void**>(address - sizeof(void*));
		*ptr = data;
		// Return pointer to aligned memory block
		return reinterpret_cast<void*>(address);
	}
	
	void operator delete (void *mem) {
		//unsigned long address = reinterpret_cast<unsigned long>(mem) - sizeof(void*);
		//::operator delete(reinterpret_cast<void*>(address));
		void** address = reinterpret_cast<void**>(reinterpret_cast<unsigned long>(mem) - sizeof(void*));
		::operator delete(reinterpret_cast<void*>(*address));
	}
	
	void operator delete[] (void *mem) {
		void** address = reinterpret_cast<void**>(reinterpret_cast<unsigned long>(mem) - sizeof(void*));
		::operator delete(reinterpret_cast<void*>(*address));
	}
};
#endif


/* bitmask for conditional assignments */
class qmask : public SIMDmemAligned
{
  public:
	qmask() {}
	qmask(__m128 packed) : packed(packed) {}
	
	/* index operators */
	unsigned int operator[](unsigned int index) const;
	unsigned int& operator[](unsigned int index);
	/* return most significant bits as 4-bit integer */
	int mask() const;
	bool allTrue() const;
	bool allFalse() const;
	/* andnot 'operator' */
	qmask andnot(const qmask&) const;
	
  private:
	union {
		__m128 packed;
		unsigned int m[4];
	};
	
	friend class qfloat;
	template <typename T, int sizeofT> friend class quad;
	
	friend const qmask& operator&=(qmask&, const qmask&);
	friend const qmask& operator|=(qmask&, const qmask&);
	friend const qmask& operator^=(qmask&, const qmask&);
};


class qfloat : public SIMDmemAligned
{
  public:
	qfloat() {}
	qfloat(float);
	
	/* index operators */
	float  operator[](unsigned int) const;
	float& operator[](unsigned int);
	
	/* assignment operator */
	const qfloat& operator=(float);
	/* conditional assignment */
	void condAssign(const qmask& mask,
					const qfloat& trueval, const qfloat& falseval);
	
  private:
	union {
		__m128 packed;
		float f[4];
	};
	
	/* arithmetic operators - in place variants */
	friend const qfloat& operator+=(qfloat&, const qfloat&);
	friend const qfloat& operator-=(qfloat&, const qfloat&);
	friend const qfloat& operator*=(qfloat&, const qfloat&);
	friend const qfloat& operator/=(qfloat&, const qfloat&);
	friend qfloat operator-(const qfloat&);
	
	/* comparison operators */
	friend qmask operator==(const qfloat& lh, const qfloat& rh);
	friend qmask operator!=(const qfloat& lh, const qfloat& rh);
	friend qmask operator< (const qfloat& lh, const qfloat& rh);
	friend qmask operator<=(const qfloat& lh, const qfloat& rh);
	friend qmask operator> (const qfloat& lh, const qfloat& rh);
	friend qmask operator>=(const qfloat& lh, const qfloat& rh);
	
	/* cmath equivalent mathematical functions */
	friend qfloat max(const qfloat&, const qfloat&);
	friend qfloat min(const qfloat&, const qfloat&);
	friend qfloat rcp(const qfloat&);
	friend qfloat sqrt(const qfloat&);
};


template <typename T, int sizeofT = sizeof(T)>
class quad;

template <typename T>
class quad<T,4> : public SIMDmemAligned
{
  public:
	quad() { assert(sizeof(T) == 4); }
	quad(T);
	quad(T, T, T, T);
	
	/* index operators */
	T  operator[](unsigned int) const;
	T& operator[](unsigned int);
	
	/* assignment operator */
	const quad<T,4>& operator=(const T);
	/* conditional assignment */
	void condAssign(const qmask& mask,
					const quad<T,4>& trueval, const quad<T,4>& falseval);
	
	/* comparison operators */
	qmask operator==(const quad<T,4>&) const;
	qmask operator!=(const quad<T,4>&) const;
	
  private:
	union {
		__m128 packed;
		T val[4];
	};
	
	// FIXME: warum funzt das nicht ?!
	//template <typename T> friend qmask operator==(const quad<T>&, const quad<T>&);
	//template <typename T> friend qmask operator!=(const quad<T>&, const quad<T>&);
};

template <typename T>
class quad<T,8> : public SIMDmemAligned
{
  public:
	  quad() { assert(sizeof(T) == 8); }
	  quad(T);
	  quad(T, T, T, T);
	
	/* index operators */
	T  operator[](unsigned int) const;
	T& operator[](unsigned int);
	
	/* assignment operator */
	const quad<T,8>& operator=(const T);
	/* conditional assignment */
	void condAssign(const qmask& mask,
					const quad<T,8>& trueval, const quad<T,8>& falseval);
	
	/* comparison operators */
	qmask operator==(const quad<T,8>&) const;
	qmask operator!=(const quad<T,8>&) const;
	
  private:
	union {
		__m128 packed[2];
		T val[4];
	};
	
	// FIXME: warum funzt das nicht ?!
	//template <typename T> friend qmask operator==(const quad<T>&, const quad<T>&);
	//template <typename T> friend qmask operator!=(const quad<T>&, const quad<T>&);
};


/***************
 * class qmask *
 ***************/

/* logical operators - in place variants */
inline const qmask& operator&=(qmask& lh, const qmask& rh) {
	lh.packed = _mm_and_ps(lh.packed, rh.packed);
	return lh;
}

inline const qmask& operator|=(qmask& lh, const qmask& rh) {
	lh.packed = _mm_or_ps(lh.packed, rh.packed);
	return lh;
}

inline const qmask& operator^=(qmask& lh, const qmask& rh) {
	lh.packed = _mm_xor_ps(lh.packed, rh.packed);
	return lh;
}

/* index operators */
inline unsigned int qmask::operator[](unsigned int index) const {
	assert(0 <= index && index < 4);
	return m[index];
}

inline unsigned int& qmask::operator[](unsigned int index) {
	assert(0 <= index && index < 4);
	return m[index];
}

/* return most significant bits as 4-bit integer */
inline int qmask::mask() const {
	return _mm_movemask_ps(packed);
}

inline bool qmask::allTrue() const {
	return mask() == 0xf /* b1111 */;
}

inline bool qmask::allFalse() const {
	return mask() == 0x0 /* b0000 */;
}

/* andnot 'operator' */
inline qmask qmask::andnot(const qmask& val) const {
	qmask temp;
	temp.packed = _mm_andnot_ps(val.packed, packed);
	return temp;
}


/****************
 * class qfloat *
 ****************/
inline qfloat::qfloat(float val)
{
	packed = _mm_load_ps1(reinterpret_cast<const float*>(&val));
}

/* arithmetic operators - in place variants */
inline const qfloat& operator+=(qfloat& lh, const qfloat& rh) {
	lh.packed = _mm_add_ps(lh.packed, rh.packed);
	return lh;
}

inline const qfloat& operator-=(qfloat& lh, const qfloat& rh) {
	lh.packed = _mm_sub_ps(lh.packed, rh.packed);
	return lh;
}

inline const qfloat& operator*=(qfloat& lh, const qfloat& rh) {
	lh.packed = _mm_mul_ps(lh.packed, rh.packed);
	return lh;
}

inline const qfloat& operator/=(qfloat& lh, const qfloat& rh) {
	lh.packed = _mm_div_ps(lh.packed, rh.packed);
	return lh;
}

inline qfloat operator-(const qfloat& rh) {
	qfloat result;
	result[0] = -rh[0];
	result[1] = -rh[1];
	result[2] = -rh[2];
	result[3] = -rh[3];
	return result;
}

/* comparison operators */
inline qmask operator==(const qfloat& lh, const qfloat& rh) {
	qmask temp( _mm_cmpeq_ps(lh.packed, rh.packed) );
	return temp;
}

inline qmask operator!=(const qfloat& lh, const qfloat& rh) {
	qmask temp( _mm_cmpneq_ps(lh.packed, rh.packed) );
	return temp;
}

inline qmask operator<(const qfloat& lh, const qfloat& rh) {
	qmask temp( _mm_cmplt_ps(lh.packed, rh.packed) );
	return temp;
}

inline qmask operator<=(const qfloat& lh, const qfloat& rh) {
	qmask temp( _mm_cmple_ps(lh.packed, rh.packed) );
	return temp;
}

inline qmask operator>(const qfloat& lh, const qfloat& rh) {
	qmask temp( _mm_cmpgt_ps(lh.packed, rh.packed) );
	return temp;
}

inline qmask operator>=(const qfloat& lh, const qfloat& rh) {
	qmask temp( _mm_cmpge_ps(lh.packed, rh.packed) );
	return temp;
}

/* index operators */
inline float qfloat::operator[](unsigned int index) const {
	assert(0 <= index && index < 4);
	return f[index];
}

inline float& qfloat::operator[](unsigned int index) {
	assert(0 <= index && index < 4);
	return f[index];
}

/* assignment operator */
inline const qfloat& qfloat::operator=(float f) {
	packed = _mm_load_ps1(&f);
	return *this;
}
/* conditional assignment */
inline void qfloat::condAssign(const qmask& mask,
							   const qfloat& trueval, const qfloat& falseval)
{
	packed = _mm_or_ps( _mm_and_ps   (mask.packed, trueval.packed),
						_mm_andnot_ps(mask.packed, falseval.packed) );
}

/* cmath equivalent mathematical functions */
inline qfloat max(const qfloat& lh, const qfloat& rh) {
	qfloat temp;
	temp.packed = _mm_max_ps(lh.packed, rh.packed);
	return temp;
}

inline qfloat min(const qfloat& lh, const qfloat& rh) {
	qfloat temp;
	temp.packed = _mm_min_ps(lh.packed, rh.packed);
	return temp;
}

inline qfloat abs(const qfloat& val) {
	// FIXME: is it save (in respect to fp format) to simply mask out sign bits ?
	qfloat zero, minusone;
	zero = 0.0f;
	minusone = -1.0f;
	qmask negative = val < zero;
	qfloat temp;
	temp.condAssign(negative, val * minusone, val);
	return temp;
}

inline qfloat sqrt(const qfloat& val) {
	qfloat temp;
	temp.packed = _mm_sqrt_ps(val.packed);
	return temp;
}

inline qfloat rcp(const qfloat& val) {
	qfloat temp;
	temp.packed = _mm_rcp_ps(val.packed);
	return temp;
}


/*****************
 * template quad *
 *****************/
template <typename T>
inline quad<T,4>::quad(T t)
{
	val[0] = val[1] = val[2] = val[3] = t;
	//packed = _mm_load_ps1(reinterpret_cast<const float*>(&t));
	//FIXME: doesn't work with -fstrict-aliasing, implied by -02 and above
	// see http://gcc.gnu.org/onlinedocs/gcc-4.1.2/gcc/Optimize-Options.html#index-fstrict_002daliasing-542
}

template <typename T>
inline quad<T,4>::quad(T t0, T t1, T t2, T t3)
{
	val[0] = t0;
	val[1] = t1;
	val[2] = t2;
	val[3] = t3;
}

/* index operators */
template <typename T>
inline T quad<T,4>::operator[](unsigned int index) const {
	assert(0 <= index && index < 4);
	return val[index];
}

template <typename T>
inline T& quad<T,4>::operator[](unsigned int index) {
	assert(0 <= index && index < 4);
	return val[index];
}

/* assignment operator */
template <typename T>
inline const quad<T,4>& quad<T,4>::operator=(const T t) {
	val[0] = val[1] = val[2] = val[3] = t;
	//packed = _mm_load_ps1(reinterpret_cast<const float*>(&t));
	//FIXME: doesn't work with -fstrict-aliasing, implied by -02 and above
	return *this;
}

/* conditional assignment */
template <typename T>
inline void quad<T,4>::condAssign(const qmask& mask,
								  const quad<T,4>& trueval, const quad<T,4>& falseval)
{
	packed = _mm_or_ps( _mm_and_ps   (mask.packed, trueval.packed),
						_mm_andnot_ps(mask.packed, falseval.packed) );
}

/* comparison operators */
template <typename T>
inline qmask quad<T,4>::operator==(const quad<T,4>& q) const {
	qmask temp( _mm_cmpeq_ps(packed, q.packed) );
	return temp;
}

template <typename T>
inline qmask quad<T,4>::operator!=(const quad<T,4>& q) const {
	qmask temp( _mm_cmpneq_ps(packed, q.packed) );
	return temp;
}
/*
template <typename T>
inline qmask operator==(const quad<T>& lh, const quad<T>& rh) {
	qmask temp( _mm_cmpeq_ps(lh.packed, rh.packed) );
	return temp;
}

template <typename T>
inline qmask operator!=(const quad<T>& lh, const quad<T>& rh) {
	qmask temp( _mm_cmpneq_ps(lh.packed, rh.packed) );
	return temp;
}
*/

template <typename T>
inline quad<T,8>::quad(T t)
{
	val[0] = val[1] = val[2] = val[3] = t;
	//packed = _mm_load_ps1(reinterpret_cast<const float*>(&t));
	//FIXME: doesn't work with -fstrict-aliasing, implied by -02 and above
	// see http://gcc.gnu.org/onlinedocs/gcc-4.1.2/gcc/Optimize-Options.html#index-fstrict_002daliasing-542
}

template <typename T>
inline quad<T,8>::quad(T t0, T t1, T t2, T t3)
{
	val[0] = t0;
	val[1] = t1;
	val[2] = t2;
	val[3] = t3;
}

/* index operators */
template <typename T>
inline T quad<T,8>::operator[](unsigned int index) const {
	assert(0 <= index && index < 4);
	return val[index];
}

template <typename T>
inline T& quad<T,8>::operator[](unsigned int index) {
	assert(0 <= index && index < 4);
	return val[index];
}

/* assignment operator */
template <typename T>
inline const quad<T,8>& quad<T,8>::operator=(const T t) {
	val[0] = val[1] = val[2] = val[3] = t;
	//packed = _mm_load_ps1(reinterpret_cast<const float*>(&t));
	//FIXME: doesn't work with -fstrict-aliasing, implied by -02 and above
	return *this;
}

/* conditional assignment */
template <typename T>
inline void quad<T,8>::condAssign(const qmask& mask,
								  const quad<T,8>& trueval, const quad<T,8>& falseval)
{
	qmask halfmask;
	halfmask.packed = _mm_shuffle_ps(mask.packed, mask.packed, _MM_SHUFFLE(1,1,0,0));
	packed[0] = _mm_or_ps( _mm_and_ps   (halfmask.packed, trueval.packed[0]),
						   _mm_andnot_ps(halfmask.packed, falseval.packed[0]) );
	halfmask.packed = _mm_shuffle_ps(mask.packed, mask.packed, _MM_SHUFFLE(3,3,2,2));
	packed[1] = _mm_or_ps( _mm_and_ps   (halfmask.packed, trueval.packed[1]),
						   _mm_andnot_ps(halfmask.packed, falseval.packed[1]) );
}

/* comparison operators */
template <typename T>
inline qmask quad<T,8>::operator==(const quad<T,8>& q) const {
	//TODO: optimizable ?
	qmask temp;
	temp.m[0] = (val[0] == q.val[0]) ? 0xffffffff : 0x0;
	temp.m[1] = (val[1] == q.val[1]) ? 0xffffffff : 0x0;
	temp.m[2] = (val[2] == q.val[2]) ? 0xffffffff : 0x0;
	temp.m[3] = (val[3] == q.val[3]) ? 0xffffffff : 0x0;
	return temp;
}

template <typename T>
inline qmask quad<T,8>::operator!=(const quad<T,8>& q) const {
	//TODO: optimizable ?
	qmask temp;
	temp.m[0] = (val[0] != q.val[0]) ? 0xffffffff : 0x0;
	temp.m[1] = (val[1] != q.val[1]) ? 0xffffffff : 0x0;
	temp.m[2] = (val[2] != q.val[2]) ? 0xffffffff : 0x0;
	temp.m[3] = (val[3] != q.val[3]) ? 0xffffffff : 0x0;
	return temp;
}
#endif
