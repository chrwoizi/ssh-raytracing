#ifndef SIMD_FPU_H
#define SIMD_FPU_H

#ifndef SIMD_INTERNAL
#error don't include simd_sse.h directly, use simd.h instead !
#endif

#include <cassert>


/*
 * when using the FPU, no specific memory alignment is required
 */
template <int alignment> class memAligned {};


/* bitmask for conditional assignments */
class qmask : public SIMDmemAligned
{
  public:
	qmask() {}
	qmask(unsigned int m0, unsigned int m1, unsigned m2, unsigned int m3)
	{
		m[0] = m0; m[1] = m1; m[2] = m2; m[3] = m3;
	}
	
	/* index operators */
	unsigned int operator[](unsigned int index) const;
	unsigned int& operator[](unsigned int index);
	/* return most significant bits as 4-bit integer */
	int mask() const;
	
  private:
	unsigned int m[4];
	
	friend class qfloat;
	template <class T> friend class quad;
	
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
	float f[4];
	
	/* arithmetic operators - in place variants */
	friend const qfloat& operator+=(qfloat&, const qfloat&);
	friend const qfloat& operator-=(qfloat&, const qfloat&);
	friend const qfloat& operator*=(qfloat&, const qfloat&);
	friend const qfloat& operator/=(qfloat&, const qfloat&);
	
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
	friend qfloat abs(const qfloat&);
	friend qfloat rcp(const qfloat&);
	friend qfloat sqrt(const qfloat&);
};


template <typename T>
class quad : public SIMDmemAligned
{
  public:
	quad() {}
	quad(T);
	
	/* index operators */
	T  operator[](unsigned int) const;
	T& operator[](unsigned int);
	
	/* assignment operator */
	const quad<T>& operator=(const T);
	/* conditional assignment */
	void condAssign(const qmask& mask,
					const quad<T>& trueval, const quad<T>& falseval);
	
  private:
	T val[4];
};


/***************
 * class qmask *
 ***************/

/* logical operators - in place variants */
inline const qmask& operator&=(qmask& lh, const qmask& rh) {
	lh.m[0] = lh.m[0] & rh.m[0];
	lh.m[1] = lh.m[1] & rh.m[1];
	lh.m[2] = lh.m[2] & rh.m[2];
	lh.m[3] = lh.m[3] & rh.m[3];
	return lh;
}

inline const qmask& operator|=(qmask& lh, const qmask& rh) {
	lh.m[0] = lh.m[0] | rh.m[0];
	lh.m[1] = lh.m[1] | rh.m[1];
	lh.m[2] = lh.m[2] | rh.m[2];
	lh.m[3] = lh.m[3] | rh.m[3];
	return lh;
}

inline const qmask& operator^=(qmask& lh, const qmask& rh) {
	lh.m[0] = lh.m[0] ^ rh.m[0];
	lh.m[1] = lh.m[1] ^ rh.m[1];
	lh.m[2] = lh.m[2] ^ rh.m[2];
	lh.m[3] = lh.m[3] ^ rh.m[3];
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


/****************
 * class qfloat *
 ****************/
inline qfloat::qfloat(float val)
{
	f[0] = f[1] = f[2] = f[3] = val;
}

/* arithmetic operators - in place variants */
inline const qfloat& operator+=(qfloat& lh, const qfloat& rh) {
	lh.f[0] = lh.f[0] + rh.f[0];
	lh.f[1] = lh.f[1] + rh.f[1];
	lh.f[2] = lh.f[2] + rh.f[2];
	lh.f[3] = lh.f[3] + rh.f[3];
	return lh;
}

inline const qfloat& operator-=(qfloat& lh, const qfloat& rh) {
	lh.f[0] = lh.f[0] - rh.f[0];
	lh.f[1] = lh.f[1] - rh.f[1];
	lh.f[2] = lh.f[2] - rh.f[2];
	lh.f[3] = lh.f[3] - rh.f[3];
	return lh;
}

inline const qfloat& operator*=(qfloat& lh, const qfloat& rh) {
	lh.f[0] = lh.f[0] * rh.f[0];
	lh.f[1] = lh.f[1] * rh.f[1];
	lh.f[2] = lh.f[2] * rh.f[2];
	lh.f[3] = lh.f[3] * rh.f[3];
	return lh;
}

inline const qfloat& operator/=(qfloat& lh, const qfloat& rh) {
	lh.f[0] = lh.f[0] / rh.f[0];
	lh.f[1] = lh.f[1] / rh.f[1];
	lh.f[2] = lh.f[2] / rh.f[2];
	lh.f[3] = lh.f[3] / rh.f[3];
	return lh;
}

/* comparison operators */
inline qmask operator==(const qfloat& lh, const qfloat& rh) {
	qmask temp( lh.f[0] == rh.f[0] ? 0xffffffff : 0x0,
				lh.f[1] == rh.f[1] ? 0xffffffff : 0x0,
				lh.f[2] == rh.f[2] ? 0xffffffff : 0x0,
				lh.f[3] == rh.f[3] ? 0xffffffff : 0x0 );
	return temp;
}

inline qmask operator!=(const qfloat& lh, const qfloat& rh) {
	qmask temp( lh.f[0] != rh.f[0] ? 0xffffffff : 0x0,
				lh.f[1] != rh.f[1] ? 0xffffffff : 0x0,
				lh.f[2] != rh.f[2] ? 0xffffffff : 0x0,
				lh.f[3] != rh.f[3] ? 0xffffffff : 0x0 );
	return temp;
}

inline qmask operator<(const qfloat& lh, const qfloat& rh) {
	qmask temp( lh.f[0] < rh.f[0] ? 0xffffffff : 0x0,
				lh.f[1] < rh.f[1] ? 0xffffffff : 0x0,
				lh.f[2] < rh.f[2] ? 0xffffffff : 0x0,
				lh.f[3] < rh.f[3] ? 0xffffffff : 0x0 );
	return temp;
}

inline qmask operator<=(const qfloat& lh, const qfloat& rh) {
	qmask temp( lh.f[0] <= rh.f[0] ? 0xffffffff : 0x0,
				lh.f[1] <= rh.f[1] ? 0xffffffff : 0x0,
				lh.f[2] <= rh.f[2] ? 0xffffffff : 0x0,
				lh.f[3] <= rh.f[3] ? 0xffffffff : 0x0 );
	return temp;
}

inline qmask operator>(const qfloat& lh, const qfloat& rh) {
	qmask temp( lh.f[0] > rh.f[0] ? 0xffffffff : 0x0,
				lh.f[1] > rh.f[1] ? 0xffffffff : 0x0,
				lh.f[2] > rh.f[2] ? 0xffffffff : 0x0,
				lh.f[3] > rh.f[3] ? 0xffffffff : 0x0 );
	return temp;
}

inline qmask operator>=(const qfloat& lh, const qfloat& rh) {
	qmask temp( lh.f[0] >= rh.f[0] ? 0xffffffff : 0x0,
				lh.f[1] >= rh.f[1] ? 0xffffffff : 0x0,
				lh.f[2] >= rh.f[2] ? 0xffffffff : 0x0,
				lh.f[3] >= rh.f[3] ? 0xffffffff : 0x0 );
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
inline const qfloat& qfloat::operator=(float fval) {
	f[0] = f[1] = f[2] = f[3] = fval;
	return *this;
}
/* conditional assignment */
inline void qfloat::condAssign(const qmask& mask,
							   const qfloat& trueval, const qfloat& falseval)
{
	// FIXME: bitwise computation, only masks with all ones or all zeros work this way
	f[0] = mask.m[0] ? trueval.f[0] : falseval.f[0];
	f[1] = mask.m[1] ? trueval.f[1] : falseval.f[1];
	f[2] = mask.m[2] ? trueval.f[2] : falseval.f[2];
	f[3] = mask.m[3] ? trueval.f[3] : falseval.f[3];
}

/* cmath equivalent mathematical functions */
inline qfloat max(const qfloat& lh, const qfloat& rh) {
	qfloat temp;
	temp.f[0] = lh.f[0] > rh.f[0] ? lh.f[0] : rh.f[0];
	temp.f[1] = lh.f[1] > rh.f[1] ? lh.f[1] : rh.f[1];
	temp.f[2] = lh.f[2] > rh.f[2] ? lh.f[2] : rh.f[2];
	temp.f[3] = lh.f[3] > rh.f[3] ? lh.f[3] : rh.f[3];
	return temp;
}

inline qfloat min(const qfloat& lh, const qfloat& rh) {
	qfloat temp;
	temp.f[0] = lh.f[0] < rh.f[0] ? lh.f[0] : rh.f[0];
	temp.f[1] = lh.f[1] < rh.f[1] ? lh.f[1] : rh.f[1];
	temp.f[2] = lh.f[2] < rh.f[2] ? lh.f[2] : rh.f[2];
	temp.f[3] = lh.f[3] < rh.f[3] ? lh.f[3] : rh.f[3];
	return temp;
}

inline qfloat abs(const qfloat& val) {
	qfloat temp;
	temp.f[0] = fabsf(val.f[0]);
	temp.f[1] = fabsf(val.f[1]);
	temp.f[2] = fabsf(val.f[2]);
	temp.f[3] = fabsf(val.f[3]);
	return temp;
}

inline qfloat sqrt(const qfloat& val) {
	qfloat temp;
	temp.f[0] = sqrtf(val.f[0]);
	temp.f[1] = sqrtf(val.f[1]);
	temp.f[2] = sqrtf(val.f[2]);
	temp.f[3] = sqrtf(val.f[3]);
	return temp;
}

inline qfloat rcp(const qfloat& val) {
	qfloat temp;
	temp.f[0] = 1.0f/val.f[0];
	temp.f[1] = 1.0f/val.f[1];
	temp.f[2] = 1.0f/val.f[2];
	temp.f[3] = 1.0f/val.f[3];
	return temp;
}


/*****************
 * template quad *
 *****************/
template <typename T>
inline quad<T>::quad(T t)
{
	val[0] = val[1] = val[2] = val[3] = t;
}

/* index operators */
template <typename T>
inline T quad<T>::operator[](unsigned int index) const {
	assert(0 <= index && index < 4);
	return val[index];
}

template <typename T>
inline T& quad<T>::operator[](unsigned int index) {
	assert(0 <= index && index < 4);
	return val[index];
}

/* assignment operator */
template <typename T>
inline const quad<T>& quad<T>::operator=(const T t) {
	val[0] = val[1] = val[2] = val[3] = t;
	return *this;
}

/* conditional assignment */
template <typename T>
inline void quad<T>::condAssign(const qmask& mask,
								const quad<T>& trueval, const quad<T>& falseval)
{
	// FIXME: bitwise computation, only masks with all ones or all zeros work this way
	val[0] = mask.m[0] ? trueval.val[0] : falseval.val[0];
	val[1] = mask.m[1] ? trueval.val[1] : falseval.val[1];
	val[2] = mask.m[2] ? trueval.val[2] : falseval.val[2];
	val[3] = mask.m[3] ? trueval.val[3] : falseval.val[3];
}
#endif
