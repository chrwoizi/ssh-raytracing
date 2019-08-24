#ifndef VECMATH_H
#define VECMATH_H

#include <cmath>

#include "simd/simd.h"


//TODO: change operators to friend functions

class vec
{
  public:
	float x,y,z;
	
	/* constructors */
	vec(void);
	vec(const float);
	vec(const float, const float, const float);
	/* index operators */
	float  operator[](unsigned int index) const;
	float& operator[](unsigned int index);
	/* arithmetic operators */
	vec operator+(const vec&) const;
	vec operator-(const vec&) const;
	vec operator*(const vec&) const;
	vec operator/(const vec&) const;
	vec operator*(float) const;
	/* arithmetic operators - in place variants */
	vec operator-() const;
	vec& operator+=(const vec&);
	vec& operator-=(const vec&);
	vec& operator*=(const vec&);
	vec& operator/=(const vec&);

	/* cross product */
	vec operator^(const vec&);

	bool operator==(const vec&);

	/* returns normalized version of this. does not change this */
	vec normalize();

	float length();
};

class qvec : public SIMDmemAligned
{
  public:
	qfloat x,y,z;
	
	/* constructors */
	qvec(void);
	qvec(const vec&);
	qvec(const qfloat&);
	/* index operators */
	qfloat  operator[](unsigned int index) const;
	qfloat& operator[](unsigned int index);
	/* arithmetic operators */
	qvec operator+(const qvec&) const;
	qvec operator-(const qvec&) const;
	qvec operator*(const qvec&) const;
	qvec operator/(const qvec&) const;
	qvec operator*(const qfloat&) const;
	qvec operator-() const;
	/* arithmetic operators - in place variants */
	qvec& operator+=(const qvec&);
	qvec& operator-=(const qvec&);
	qvec& operator*=(const qvec&);
	qvec& operator/=(const qvec&);
	/* assignment operator */
	const qvec& operator=(const vec& v);
};



/*************
 * class vec *
 *************/
 
/* constructors */
inline vec::vec() {}

inline vec::vec(const float f)
: x(f),y(f),z(f)
{}

inline vec::vec(const float x, const float y, const float z)
: x(x),y(y),z(z)
{}

/* index operators */
inline float vec::operator[](unsigned int index) const {
	assert(0 <= index && index < 4);
	return *(&x + index);
}

inline float& vec::operator[](unsigned int index) {
	assert(0 <= index && index < 4);
	return *(&x + index);
}

/* arithmetic operators */
inline vec vec::operator+(const vec& rh) const {
	vec temp;
	temp.x = x + rh.x;
	temp.y = y + rh.y;
	temp.z = z + rh.z;
	return temp;
}

inline vec vec::operator-(const vec& rh) const {
	vec temp;
	temp.x = x - rh.x;
	temp.y = y - rh.y;
	temp.z = z - rh.z;
	return temp;
}

inline vec vec::operator*(const vec& rh) const {
	vec temp;
	temp.x = x * rh.x;
	temp.y = y * rh.y;
	temp.z = z * rh.z;
	return temp;
}

inline vec vec::operator/(const vec& rh) const {
	vec temp;
	temp.x = x / rh.x;
	temp.y = y / rh.y;
	temp.z = z / rh.z;
	return temp;
}

inline vec vec::operator*(float rh) const {
	vec temp;
	temp.x = x * rh;
	temp.y = y * rh;
	temp.z = z * rh;
	return temp;
}

inline vec operator*(float rh, const vec& v) {
	vec temp;
	temp.x = v.x * rh;
	temp.y = v.y * rh;
	temp.z = v.z * rh;
	return temp;
}

inline vec vec::operator-() const {
	vec temp;
	temp.x = -x;
	temp.y = -y;
	temp.z = -z;
	return temp;
}

/* arithmetic operators - in place variants */
inline vec& vec::operator+=(const vec& rh) {
	x += rh.x;
	y += rh.y;
	z += rh.z;
	return *this;
}

inline vec& vec::operator-=(const vec& rh) {
	x -= rh.x;
	y -= rh.y;
	z -= rh.z;
	return *this;
}

inline vec& vec::operator*=(const vec& rh) {
	x *= rh.x;
	y *= rh.y;
	z *= rh.z;
	return *this;
}

inline vec& vec::operator/=(const vec& rh) {
	x /= rh.x;
	y /= rh.y;
	z /= rh.z;
	return *this;
}

inline float dot(const vec& a, const vec& b) {
	return a.x*b.x + a.y*b.y + a.z*b.z;
}

inline vec cross(const vec& a, const vec& b) {
	vec c;
	c.x = a.y*b.z - a.z*b.y;
	c.y = a.z*b.x - a.x*b.z;
	c.z = a.x*b.y - a.y*b.x;
	return c;
}

inline float length(const vec& v) {
	return sqrtf(dot(v,v));
}

inline void normalize(vec &v) {
	v /= length(v);
}

inline vec vec::operator^(const vec& rh) {
	return cross(*this, rh);
}

inline bool vec::operator==(const vec& rh) {
	return (x == rh.x) && (y == rh.y) && (z == rh.z);
}

inline vec vec::normalize()
{
	vec result = *this;
	::normalize(result);
	return result;
}

inline float vec::length()
{
	return ::length(*this);
}


/**************
 * class qvec *
 **************/

/* constructors */
inline qvec::qvec() {}

inline qvec::qvec(const vec& v)
//: x(v.x),y(v.y),z(v.z) TODO!
{
	x = v.x;
	y = v.y;
	z = v.z;
}

inline qvec::qvec(const qfloat& q)
{
	x[0] = y[0] = z[0] = q[0];
	x[1] = y[1] = z[1] = q[1];
	x[2] = y[2] = z[2] = q[2];
	x[3] = y[3] = z[3] = q[3];
}

/* index operators */
inline qfloat qvec::operator[](unsigned int index) const {
	assert(0 <= index && index < 4);
	return *(&x + index);
}

inline qfloat& qvec::operator[](unsigned int index) {
	assert(0 <= index && index < 4);
	return *(&x + index);
}

/* arithmetic operators */
inline qvec qvec::operator+(const qvec& rh) const {
	qvec temp;
	temp.x = x + rh.x;
	temp.y = y + rh.y;
	temp.z = z + rh.z;
	return temp;
}

inline qvec qvec::operator-(const qvec& rh) const {
	qvec temp;
	temp.x = x - rh.x;
	temp.y = y - rh.y;
	temp.z = z - rh.z;
	return temp;
}

inline qvec qvec::operator*(const qvec& rh) const {
	qvec temp;
	temp.x = x * rh.x;
	temp.y = y * rh.y;
	temp.z = z * rh.z;
	return temp;
}

inline qvec qvec::operator/(const qvec& rh) const {
	qvec temp;
	temp.x = x / rh.x;
	temp.y = y / rh.y;
	temp.z = z / rh.z;
	return temp;
}

inline qvec qvec::operator*(const qfloat& rh) const {
	qvec temp;
	temp.x = x * rh;
	temp.y = y * rh;
	temp.z = z * rh;
	return temp;
}

inline qvec qvec::operator-() const {
	qvec temp;
	temp.x = -x;
	temp.y = -y;
	temp.z = -z;
	return temp;
}

/* arithmetic operators - in place variants */
inline qvec& qvec::operator+=(const qvec& rh) {
	x += rh.x;
	y += rh.y;
	z += rh.z;
	return *this;
}

inline qvec& qvec::operator-=(const qvec& rh) {
	x -= rh.x;
	y -= rh.y;
	z -= rh.z;
	return *this;
}

inline qvec& qvec::operator*=(const qvec& rh) {
	x *= rh.x;
	y *= rh.y;
	z *= rh.z;
	return *this;
}

inline qvec& qvec::operator/=(const qvec& rh) {
	x /= rh.x;
	y /= rh.y;
	z /= rh.z;
	return *this;
}

/* assignment operator */
inline const qvec& qvec::operator=(const vec& rh) {
	x = rh.x;
	y = rh.y;
	z = rh.z;
	return *this;
}

inline qfloat dot(const qvec& a, const qvec& b) {
	return a.x*b.x + a.y*b.y + a.z*b.z;
}

inline qvec cross(const qvec& a, const qvec& b) {
	qvec c;
	c.x = a.y*b.z - a.z*b.y;
	c.y = a.z*b.x - a.x*b.z;
	c.z = a.x*b.y - a.y*b.x;
	return c;
}

inline qfloat length(const qvec& v) {
	return sqrt(dot(v,v));
}

inline void normalize(qvec &v) {
	v /= length(v);
}
#endif
