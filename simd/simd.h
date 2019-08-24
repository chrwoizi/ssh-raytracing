#ifndef SIMD_H
#define SIMD_H

class qmask;
/* logical operators */
qmask operator&(const qmask&, const qmask&);
qmask operator|(const qmask&, const qmask&);
qmask operator^(const qmask&, const qmask&);
/* logical operators - in place variants */
const qmask& operator&=(qmask&, const qmask&);
const qmask& operator|=(qmask&, const qmask&);
const qmask& operator^=(qmask&, const qmask&);


class qfloat;
/* arithmetic operators */
qfloat operator+(const qfloat&, const qfloat&);
qfloat operator-(const qfloat&, const qfloat&);
qfloat operator*(const qfloat&, const qfloat&);
qfloat operator/(const qfloat&, const qfloat&);
/* arithmetic operators - in place variants */
const qfloat& operator+=(qfloat&, const qfloat&);
const qfloat& operator-=(qfloat&, const qfloat&);
const qfloat& operator*=(qfloat&, const qfloat&);
const qfloat& operator/=(qfloat&, const qfloat&);
/* comparison operators */
qmask operator==(const qfloat&, const qfloat&);
qmask operator!=(const qfloat&, const qfloat&);
qmask operator< (const qfloat&, const qfloat&);
qmask operator<=(const qfloat&, const qfloat&);
qmask operator> (const qfloat&, const qfloat&);
qmask operator>=(const qfloat&, const qfloat&);
/* cmath equivalent mathematical functions */
qfloat max(const qfloat&, const qfloat&);
qfloat min(const qfloat&, const qfloat&);
qfloat abs(const qfloat&);
qfloat sqrt(const qfloat&);
qfloat rcp(const qfloat&);


template <typename T, int sizeofT> class quad;


template <int alignment> class memAligned;
typedef memAligned<16> SIMDmemAligned;


#define SIMD_INTERNAL

#ifdef SIMD_USE_FPU
#error simd_fpu.h needs to be updated to reflect latest changes in simd_sse.h !
#include "simd_fpu.h"
#elif defined SIMD_USE_SSE
#include "simd_sse.h"
#else
#error no instruction set for SIMD operations defined !
#endif

#undef SIMD_INTERNAL


/******************
 * implementation *
 ******************/

/* qmask - logical operators */
inline qmask operator&(const qmask& lh, const qmask& rh) {
	qmask temp(lh);
	temp &= rh;
	return temp;
}

inline qmask operator|(const qmask& lh, const qmask& rh) {
	qmask temp(lh);
	temp |= rh;
	return temp;
}

inline qmask operator^(const qmask& lh, const qmask& rh) {
	qmask temp(lh);
	temp ^= rh;
	return temp;
}

/* qfloat - arithmetic operators */
inline qfloat operator+(const qfloat& lh, const qfloat& rh) {
	qfloat temp(lh);
	temp += rh;
	return temp;
}

inline qfloat operator-(const qfloat& lh, const qfloat& rh) {
	qfloat temp(lh);
	temp -= rh;
	return temp;
}

inline qfloat operator*(const qfloat& lh, const qfloat& rh) {
	qfloat temp(lh);
	temp *= rh;
	return temp;
}

inline qfloat operator/(const qfloat& lh, const qfloat& rh) {
	qfloat temp(lh);
	temp /= rh;
	return temp;
}

#endif
