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


#include "bigfloat.h"

BigFloat::BigFloat() : data(NULL)
{
	data = (unsigned int*)malloc(sizeof(int)*PRECISION);
	if(data == NULL)
		printf("ERROR could not malloc\n");

	set_with_double(1.0);	
}

BigFloat::BigFloat(double a) : data(NULL)
{
	data = (unsigned int*)malloc(sizeof(int)*PRECISION);
	if(data == NULL)
		printf("ERROR could not malloc\n");

	set_with_double(a);	
}


BigFloat::BigFloat(char * hex_data, int exponent, int sign) : data(NULL)
{
	data = (unsigned int*)malloc(sizeof(int)*PRECISION);
	if(data == NULL)
		printf("ERROR could not malloc\n");

	set_with_double(1.0);	

	printf("Not implemented yet\n\n");
}

BigFloat::BigFloat(const BigFloat& cc) : data(NULL)
{
	this->data = (unsigned int*)malloc(sizeof(int)*PRECISION);
	if(data == NULL)
		printf("ERROR could not malloc\n");
	
	memcpy(this->data, cc.data, sizeof(int)*PRECISION);
	this->exp = cc.exp;
	this->sign = cc.sign;
}

int BigFloat::set_with_double(double a)
{
	for(int i=0; i<PRECISION; i++)
		data[i] = 0;
	
	int * iptr;
	double * dptr;

	int t1;
	int t2;
	int t3;

	dptr = &a;
	iptr = (int *)dptr;

	t1 = *(iptr+1);
	t2 = *(iptr+0);
	t3 = *(iptr+0);
	//printf("t1: %x\n\n", t1);

	sign =1;
	if(t1&0x80000000)
		sign = -1;

	t1 = t1&0x7FFFFFFF;
	
	//printf("t1: %x\n\n", t1);

	exp = t1&0xFFF00000;
	exp = exp>>20;
	exp -= 0x000003FF;
	
	//printf("exp: %d", exp);

	t1 = t1&0x000FFFFF;
	if(a!=0.0)
		t1 = t1|0x00100000;
	t1 = t1 << 11;
	t1 = t1 & 0xFFFFF800;

	t2 = t2 >> 21; //  is this right?
	t2 = t2 & 0x000007FF;

	t3 = t3 << 11;
	t3 = t3 & 0xFFFFF800;
	
	//printf("T2: %x\n\n", t2);
	data[0] = t1+t2;
	data[1] = t3;

	//printf("t1: %x\n\n", t1);

	return 0;
}

int BigFloat::printAll()
{

	printf("Exponent: %d    Sign:%d\n", exp, sign);
	printf("  data: %x %x  %x  %x\n", data[0], data[1], data[2], data[3]);
	double t=getdouble();
	double *dptr = &t;
	int *iptr = (int *)dptr;
	printf("  double %f     %e  in hex  %x %x\n\n", (float)t,(float)t, (*(iptr+1)), (*(iptr+0)));
	return 0;
}

int BigFloat::print()
{
	for(int i=0; i<PRECISION; i+=4)
	{
		printf("  %x \t %x \t %x \t %x\n", data[i+0], data[i+1], data[i+2], data[i+3]);
	}
	return 0;
}

int BigFloat::is_zero()
{
	for(int i=0; i<PRECISION; i++)
	{
		if(data[i] != 0)
			return 0;
	}
	return 1;
}
int BigFloat::copy(class BigFloat * a)
{
	for(int i=0; i<PRECISION; i++)
		data[i] = a->data[i];
	exp = a->exp;
	sign = a->sign;
	return 0;
}

int BigFloat::multiply_by_negative_one()
{
	sign = sign*-1;

	return 1;
}

int BigFloat::multiply(class BigFloat * a, class BigFloat * b)
{
	BigFloat* sum;
	sum = new BigFloat(0.0);
	BigFloat* copy;
	copy = new BigFloat(0.0);

	if(a->is_zero() || b->is_zero())
	{
		set_with_double(0.0);
		return 0;
	}

	int bexp = b->exp;


	b->exp = 1;

	sum->copy(b);
	
	//printf("ok here we go.\n\n");

	for(int i=1; i<PRECISION*32; i++)
	{
		//printf("%x\n", 1<<(i%32));
		if(a->data[i/32] & 1<<(31-i%32))
		{
			//printf("hi ");
			b->exp = 1-i;
			copy->copy(sum);
			sum->add_same_sign(copy, b);
			/*printf("ADD");
			b->printAll();
			copy->printAll();
			printf("equals\n");
			sum->printAll();
			printf("DONE ADD\n\n");*/
		}
	}
	b->exp = bexp;
	sum->exp = bexp + a->exp + sum->exp-1;
	//printf("BEXP: %d", bexp);
	//sum->printAll();
	//printf("done");

	// above sum->copy(b);
	// we use sum because I wanted the following to work a->multiply(a,b) and by using sum it garantees that will work
	// Initializing and allocating space for sum slows it down but and I haven't checked the above with out using it.
	sign = a->sign*b->sign;
	exp = sum->exp;

	for(int i=0; i<PRECISION; i++)
	{
		data[i] = sum->data[i];
	}

	delete sum;
	delete copy;

	return 0;
}

int BigFloat::divide(class BigFloat * a, class BigFloat * b)
{
	if(b->is_zero())
	{
		printf("ERROR DIVIDE BY ZERO\n\n");
		return -1;
	}

	BigFloat * t;
	t = new BigFloat(1.0);

	t->reciprocal(b);

	//printf("Trevor: %f   %f\n\n", b->getdouble(), t->getdouble());

	multiply(a,t);

	delete t;

	return 0;
}
int BigFloat::reciprocal(class BigFloat * a)
{
	if(a->is_zero())
	{
		printf("\n\nDIVIDE BY ZERO ERROR\n\n");
		return -1;
	}

	BigFloat* one;
	one = new BigFloat(1.0);

	int same =0;
	if(a == this)
	{
		same =1;
		a = new BigFloat(1.0);
		a->copy(this);
	}

	int saveexponent = a->exp;
	int savesign = a->sign;
	exp = a->exp*-1;
	for(int i=0; i<PRECISION; i++)
		data[i] = 0;

	a->exp=0;
	a->sign=-1;



	//for(int i=0; i<320; i++)
	while( (a->exp*-1<PRECISION*32) && (one->is_zero() == 0) )
	{
		if(a->compare(one) == -1)
		{
			//printf("ha ha\n");
			a->exp--;
		}

		int t = ((a->exp*-1)/32);
		if(t<PRECISION)
		{
			data[t] = data[t] | 1<<(31-(a->exp*-1)%32);

			one->add(one,a);
			a->exp = one->exp;
		}
		//printf("%d", a->exp);

		//one->printAll();
	}


	a->sign = savesign;// we need to return a to its original state?
	sign = a->sign;
	a->exp = saveexponent;
	fix_exp();
	//printf("\n\n");
	//printAll();

	if(same ==1)
		delete a;

	delete one;

	return 0;
}
int BigFloat::add(class BigFloat * a, class BigFloat * b)
{
	if(a->is_zero())
	{
		copy(b);
		return 0;
	}

	if(b->is_zero())
	{
		copy(a);
		return 0;
	}

	int del_a=0;
	int del_b=0;
	// the bigger one gets copied later on but too lazy to check which it is here so just copy both if needed
	if(this == a) // sort of interesting case 
	{
		a = new BigFloat(1.0);
		a->copy(this);
		del_a = 1;
	}

	if(this == b)
	{
		b = new BigFloat(1.0);
		b->copy(this);
		del_b = 1;
	}

	if(a->sign == b->sign)
	{
		//printf("ADD SAME SIGN");
		add_same_sign(a,b);
		sign = a->sign;
	}
	else
	{
		//printf("SUBTRACTION         \n");
		if(a->exp == b->exp)
		{
			if(a->compare(b) == -1)
			{
				//printf("\n\n\n B IS SMALLER");
				//a->printAll();
				//b->printAll();
				//printf("\n\n\n\n\n END");
				add_diff_sign(a,b);
				sign = a->sign;
			}
			else
			{
				//printf("\n\n\n A IS SMALLER");
				//a->printAll();
				//b->printAll();
				//printf("\n\n\n\n\n END");
				add_diff_sign(b,a);
				sign = b->sign;
			}
		}
		else
		{
			if(a->exp > b->exp)
			{
				add_diff_sign(a,b);
				sign = a->sign;
			}
			else
			{
				add_diff_sign(b,a);
				sign = b->sign;
			}
		}
	}

	if(del_a)
		delete a;
	if(del_b)
		delete b;

	return 0;
}
int BigFloat::subtract(class BigFloat * a, class BigFloat * b)
{
	b->sign *= -1;

	add(a,b);
	
	b->sign *= -1;

	return 0;
}

int BigFloat::add_same_sign(class BigFloat * a, class BigFloat * b)
{
	unsigned int t;
	int expdiff;
	int expoffset;
	unsigned short * sptrA;
	unsigned int * iptrA;
	unsigned short * sptrB;
	unsigned int * iptrB;
	BigFloat *temp;
	int carry;
	if(b->exp > a->exp)
	{
		temp = a;
		a=b;
		b=temp;
	}
	copy(a);

	expoffset = exp-b->exp;
	expdiff = expoffset%16;
	expoffset /= 16;

	//printf("Simple ADD\n");
	//printf("expoffset: %d \n\n", expoffset);
	//printf("expdiff: %d \n\n", expdiff);


	iptrA = &data[0];
	sptrA = ((unsigned short *)iptrA);

	iptrB = &(b->data[0]);
	sptrB = ((unsigned short *)iptrB);


	// | LSB  MSB | LSB  MSB | LSB  MSB |
	// ----------- ---------- ----------
	//
	//  
	//
	//loop
	carry = 0;

	int i=0;
	for(; i<PRECISION*2-expoffset; i++)
	{
		if((PRECISION*2-(i+expoffset+1))-((i+expoffset)%2)*2 < 0)
			t=*(sptrB+(PRECISION*2-(i+expoffset))-((i+1+expoffset)%2)*2);
		else
		{
			t=*(sptrB+(PRECISION*2-(i+expoffset+1))-((i+expoffset)%2)*2);
			//printf("HERE: %d\n", (PRECISION*2-(i+expoffset))-((i+1+expoffset)%2)*2);
			t=t<<16;
			t+=*(sptrB+(PRECISION*2-(i+expoffset))-((i+1+expoffset)%2)*2);
		}
		
		//printf("ok: %X   ",t);
		t = t << (16-expdiff);
		t = t >> (16);
		t = t&0x0000FFFF;

		t+=carry;
		carry = 0;


		t = t+*(sptrA+(PRECISION*2-i)-((i+1)%2)*2);
		if(t>0x0000FFFF)
			carry=1;
		t=t&0x0000FFFF;
		*(sptrA+(PRECISION*2-i)-((i+1)%2)*2) = (unsigned short)t;
	}

	if(carry == 1)// not entirely sure about this part
	{
		for(; i<PRECISION*2 && carry == 1; i++)
		{
			*(sptrA+(PRECISION*2-i)-((i+1)%2)*2) += 1;
			if(*(sptrA+(PRECISION*2-i)-((i+1)%2)*2) != 0)
				carry=0;
		}
	}

	if(carry == 1)
	{
		exp++;
		unsigned int shift2=0;
		unsigned int shift=0;
		for(int i=0; i<PRECISION; i++)
		{
			shift = data[i] & 0x00000001;
			data[i] = data[i] >> 1;
			data[i] = data[i] & 0x7FFFFFFF;
			if(shift2)
				data[i] = data[i] | 0x80000000;
			shift2 = shift;
		}
	}data[0] = data[0] | 0x80000000;
	
	fix_exp();

	//printf("END ADD\n\n");
	return 0;
}


int BigFloat::add_diff_sign(class BigFloat * a, class BigFloat * b)
{
	unsigned int t;
	unsigned int tt;
//	signed int t2;
	int expdiff;
	int expoffset;
	unsigned short * sptrA;
	unsigned int * iptrA;
	unsigned short * sptrB;
	unsigned int * iptrB;
	//BigFloat *temp;
	int carry;

	//printf("ADD DIFF SIGN\n\n");
	copy(a);

	expoffset = exp-b->exp;
	expdiff = expoffset%16;
	expoffset /= 16;

	//printf("expoffset: %d \n\n", expoffset);
	//printf("expdiff: %d \n\n", expdiff);

	iptrA = &data[0];
	sptrA = ((unsigned short *)iptrA);

	iptrB = &(b->data[0]);
	sptrB = ((unsigned short *)iptrB);

	// | LSB  MSB | LSB  MSB | LSB  MSB |
	// ----------- ---------- ----------
	carry = 0;

	int i=0;
	for(; i<PRECISION*2-expoffset; i++)
	{
		//printf("i: %d\n", i);
		if((PRECISION*2-(i+expoffset+1))-((i+expoffset)%2)*2 < 0)
			t=*(sptrB+(PRECISION*2-(i+expoffset))-((i+1+expoffset)%2)*2);
		else
		{
		//	printf("%d\n\n", (PRECISION*2-(expoffset+1))-((expoffset)%2)*2);
			t=*(sptrB+(PRECISION*2-(i+expoffset+1))-((i+expoffset)%2)*2);
			//printf("HERE: %d\n", (PRECISION*2-(i+expoffset))-((i+1+expoffset)%2)*2);
			t=t<<16;
			t+=*(sptrB+(PRECISION*2-(i+expoffset))-((i+1+expoffset)%2)*2);
		}
		
		//printf("ok: %X   \n",t);
		t = t << (16-expdiff);
		t = t >> (16);
		t = t&0x0000FFFF;

		if(carry)
		{
			if(*(sptrA+(PRECISION*2-i)-((i+1)%2)*2) == 0)
			{
				*(sptrA+(PRECISION*2-i)-((i+1)%2)*2) = 0xFFFF;
			}
			else
			{
				*(sptrA+(PRECISION*2-i)-((i+1)%2)*2) -= carry;
				carry=0;
			}
		}

		//printf("ok  : %X   \n", *(sptrA+(PRECISION*2-i)-((i+1)%2)*2));
		if(*(sptrA+(PRECISION*2-i)-((i+1)%2)*2) < t)
		{
			tt = *(sptrA+(PRECISION*2-i)-((i+1)%2)*2);
			tt += 0x00010000;
			tt -= t;
			carry = 1;
			*(sptrA+(PRECISION*2-i)-((i+1)%2)*2) = tt;
		}
		else
			*(sptrA+(PRECISION*2-i)-((i+1)%2)*2) -= t;
		//printf("done: %X   \n", *(sptrA+(PRECISION*2-i)-((i+1)%2)*2));
	}


	if(carry == 1)// not entirely sure about this part
	{
		for(; i<PRECISION*2 && carry == 1; i++)
		{
			//printf("i: %d  %X\n\n", i, *(sptrA+(PRECISION*2-i)-((i+1)%2)*2));
			if(*(sptrA+(PRECISION*2-i)-((i+1)%2)*2) != 0)
			{
				*(sptrA+(PRECISION*2-i)-((i+1)%2)*2) -= 1;
				carry=0;
			}
			else
				*(sptrA+(PRECISION*2-i)-((i+1)%2)*2) = 0xFFFF;
		}
	}


	if(is_zero())
	{
		//printf("ITS ZERO");
	}
	else
	fix_exp();


	return 0;
}

int BigFloat::save_to_disk(char *filename)
{
	FILE *fh;
	fh = fopen(filename, "w+b");

	if(fh == NULL)
		return -1;

	int size = PRECISION;

	fwrite(&size, sizeof(int), 1, fh);
	fwrite(&sign, sizeof(int), 1, fh);
	fwrite(&exp, sizeof(int), 1, fh);
	for(int i=0; i<PRECISION; i+=4)
		fwrite(&(data[i]), sizeof(int), 4, fh);
	
	fclose(fh);

	return 0;
}

int BigFloat::load_from_disk(char *filename)
{
	FILE *fh;
	fh = fopen(filename, "r+b");

	if(fh == NULL)
	{
		printf("Could not open %s\n\n", filename);
		return -1;
	}

	int size;
	fread(&size, sizeof(int), 1, fh);

	if(size != PRECISION)
	{
		printf("PRECISION MISMATCH, size in file = %d\n\n", size);
		fclose(fh);
		return -1;
	}

	fread(&sign, sizeof(int), 1, fh);
	fread(&exp, sizeof(int), 1, fh);

	for(int i=0; i<PRECISION; i+=4)
		fread(&(data[i]), sizeof(int),4,fh);

	fclose(fh);
	return 0;
}
int BigFloat::fix_exp()
{
	unsigned int shift;
	unsigned int shift2=0;
	while( (data[0] & 0x80000000) == 0)
	{
		exp--;
		for(int i=PRECISION-1; i>=0; i--)
		{
			shift = data[i] & 0x80000000;
			data[i] = data[i] << 1;
			data[i] = data[i] & 0xFFFFFFFE; // make sure 0<<1 != 1
			data[i] = data[i] | shift2;
			if(shift)
				shift2 = 1;
			else
				shift2 = 0;
			//printf("CHECK %d  : %x\n\n", i, data[i]);
		}
		shift2=0;
	}


	return 0;
}


int BigFloat::get_exp()
{
	return exp;
}

int BigFloat::get_sign()
{
	return sign;
}

double BigFloat::getdouble()
{
	int *p;
	double *out;
	int d[2];
	p = &d[0];
	out = (double *)p;
	int t = data[0] & 0x7FFFFFFF;//it hates this statement since it always crashes here

	if(exp > 80)
		return 0.0;
	if(is_zero())
		return 0.0;

	
	d[0] = 0;
  // zomg what about double overflow

	d[1] = exp+0x3ff;
	d[1] = d[1] << 20;
	//printf("ok: %x\n\n", d[1]);
	t= t>>11;
	d[1] = d[1] | t;

	d[0] = data[0]<<21;
	d[0] += (data[1]>>11);
	//printf("HERE HJ: %x", data[1] >> 11);

	if(sign < 0)
		*out *= -1;
	
	return *out;
}

float BigFloat::getfloat()
{
	float a = getdouble();
	return a;
}
int BigFloat::compare(class BigFloat * a)
{
	//if(sign != a->sign)
	//	return -2;
	if(exp != a->exp)
	{
		if(a->exp > exp)
			return 1;
		return -1;
	}
	for(int i=0; i<PRECISION; i++)
	{
		if(data[i] != a->data[i])
		{
			if(a->data[i] > data[i])
				return 1;
			else
				return -1;
		}
	}
	
	return 0;
}

int BigFloat::compare_sign(class BigFloat * a)
{
	if(a->sign != sign)
		return -1;
	return 1;
}

BigFloat::~BigFloat()
{
	if(data) free(data);
}



