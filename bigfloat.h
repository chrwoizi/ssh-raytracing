// ==============================================================
//
//                      Trevor Q Foley
//                      Class BigFloat
//                      http:www.tqfoley.com
//
//  ------------------------------------------------------------
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// ==============================================================

#pragma once

#include <stdio.h>

#include <malloc.h>
#include <string.h>


// Change this for more precision 
// number of 32 bit chunks for significand
// 100 would mean 3200 bits of precision
#define PRECISION 8

class BigFloat
{
private:
	unsigned int *data;
	int exp;
	int sign;

private:
	int add_same_sign(class BigFloat * a, class BigFloat * b);
	int add_diff_sign(class BigFloat * a, class BigFloat * b);
	int add_diff_sign2(class BigFloat * a, class BigFloat * b);
	int fix_exp();
	
public:
	BigFloat();
	BigFloat(double a);
	BigFloat(char * hex_data, int exponent, int sign);
	BigFloat(const BigFloat& cc);

	int copy(class BigFloat * a);	
	int set_with_double(double a);
	int is_zero();

	int save_to_disk(char * filename);
	int load_from_disk(char * filename);

	int multiply_by_negative_one();
	int add(class BigFloat * a, class BigFloat * b);
	int subtract(class BigFloat * a, class BigFloat * b);
	int multiply(class BigFloat * a, class BigFloat * b);
	int divide(class BigFloat * a, class BigFloat * b);
	int reciprocal(class BigFloat * a);
	int compare(class BigFloat * a);
	int compare_sign(class BigFloat * a); // returns 1 if signs are the same, -1 if different

	int get_exp();
	int get_sign();
	
	double getdouble();
	float getfloat();
	
	int print();
	int printAll();

	BigFloat operator*(BigFloat other)
	{
		BigFloat result;
		result.multiply(this, &other);
		return result;
	}

	BigFloat operator/(BigFloat other)
	{
		BigFloat result;
		result.divide(this, &other);
		return result;
	}

	BigFloat operator+(BigFloat other)
	{
		BigFloat result;
		result.add(this, &other);
		return result;
	}

	BigFloat operator-(BigFloat other)
	{
		BigFloat result;
		result.subtract(this, &other);
		return result;
	}

	BigFloat& operator+=(BigFloat other)
	{
		add(this, &other);
		return *this;
	}

	BigFloat& operator=(const BigFloat& other)
	{
		memcpy(this->data, other.data, sizeof(int)*PRECISION);
		this->exp = other.exp;
		this->sign = other.sign;
		return *this;
	}
	
	~BigFloat();
};
