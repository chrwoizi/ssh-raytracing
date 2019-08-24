#ifndef SIMDQUADTEST_H
#define SIMDQUADTEST_H

#include <cppunit/extensions/HelperMacros.h>

#include "simd.h"


class SIMDQuadTest : public CppUnit::TestFixture, public SIMDmemAligned {

	CPPUNIT_TEST_SUITE( SIMDQuadTest );
	CPPUNIT_TEST( testCondAssign );
	CPPUNIT_TEST( testComparisonEqual );
	CPPUNIT_TEST( testComparisonNotEqual );
	CPPUNIT_TEST( testCondAssign64 );
	CPPUNIT_TEST( testComparisonEqual64 );
	CPPUNIT_TEST( testComparisonNotEqual64 );
	CPPUNIT_TEST_SUITE_END();
	
  public:
	void setUp()
	{	
		q1[0] = one;
		q1[1] = two;
		q1[2] = three;
		q1[3] = four;
		
		q2[0] = five;
		q2[1] = six;
		q2[2] = seven;
		q2[3] = eight;
		
		qm[0] = 0x00000000;
		qm[1] = 0xffffffff;
		qm[2] = 0x00000000;
		qm[3] = 0xffffffff;
		
		ql1[0] = 0x000d000c000b000aLL;
		ql1[1] = 0x000d000c000b000aLL;
		ql1[2] = 0x0004000300020001LL;
		ql1[3] = 0x0004000300020001LL;
		
		ql2[0] = 0x000d000c000b000aLL;
		ql2[1] = 0x0004000300020001LL;
		ql2[2] = 0x000d000c000b000aLL;
		ql2[3] = 0x0004000300020001LL;
	}
	
	void tearDown() {}
	
	void testCondAssign()
	{
		quad<numbers> result;
		result.condAssign(qm, q1, q2);
		CPPUNIT_ASSERT( result[0] == q2[0] );
		CPPUNIT_ASSERT( result[1] == q1[1] );
		CPPUNIT_ASSERT( result[2] == q2[2] );
		CPPUNIT_ASSERT( result[3] == q1[3] );
	}
	
	void testComparisonEqual()
	{
		qmask result = q1 == q2;
		CPPUNIT_ASSERT( result[0] == ((q1[0] == q2[0]) ? 0xffffffff : 0x00000000) );
		CPPUNIT_ASSERT( result[1] == ((q1[1] == q2[1]) ? 0xffffffff : 0x00000000) );
		CPPUNIT_ASSERT( result[2] == ((q1[2] == q2[2]) ? 0xffffffff : 0x00000000) );
		CPPUNIT_ASSERT( result[3] == ((q1[3] == q2[3]) ? 0xffffffff : 0x00000000) );
	}
	
	void testComparisonNotEqual()
	{
		qmask result = q1 != q2;
		CPPUNIT_ASSERT( result[0] == ((q1[0] != q2[0]) ? 0xffffffff : 0x00000000) );
		CPPUNIT_ASSERT( result[1] == ((q1[1] != q2[1]) ? 0xffffffff : 0x00000000) );
		CPPUNIT_ASSERT( result[2] == ((q1[2] != q2[2]) ? 0xffffffff : 0x00000000) );
		CPPUNIT_ASSERT( result[3] == ((q1[3] != q2[3]) ? 0xffffffff : 0x00000000) );
	}
	
	void testCondAssign64()
	{
		quad<long long> result;
		result.condAssign(qm, ql1, ql2);
		CPPUNIT_ASSERT( result[0] == ql2[0] );
		CPPUNIT_ASSERT( result[1] == ql1[1] );
		CPPUNIT_ASSERT( result[2] == ql2[2] );
		CPPUNIT_ASSERT( result[3] == ql1[3] );
	}
	
	void testComparisonEqual64()
	{
		qmask result = (ql1 == ql2);
		CPPUNIT_ASSERT( result[0] == (ql1[0] == ql2[0]) );
		CPPUNIT_ASSERT( result[1] == (ql1[1] == ql2[1]) );
		CPPUNIT_ASSERT( result[2] == (ql1[2] == ql2[2]) );
		CPPUNIT_ASSERT( result[3] == (ql1[3] == ql2[3]) );
	}
	
	void testComparisonNotEqual64()
	{
		qmask result = ql1 != ql2;
		CPPUNIT_ASSERT( result[0] == (ql1[0] != ql2[0]) );
		CPPUNIT_ASSERT( result[1] == (ql1[1] != ql2[1]) );
		CPPUNIT_ASSERT( result[2] == (ql1[2] != ql2[2]) );
		CPPUNIT_ASSERT( result[3] == (ql1[3] != ql2[3]) );
	}
	
  private:
	enum numbers {
		one,
		two,
		three,
		four,
		five,
		six,
		seven,
		eight
	};
	quad<numbers> q1, q2;
	qmask qm;
	quad<long long> ql1, ql2;
};

#endif
