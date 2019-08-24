#ifndef SIMDMASKTEST_H
#define SIMDMASKTEST_H

#include <cppunit/extensions/HelperMacros.h>

#include "simd.h"


class SIMDMaskTest : public CppUnit::TestFixture, public SIMDmemAligned {

	CPPUNIT_TEST_SUITE( SIMDMaskTest );
	CPPUNIT_TEST( testAnd );
	CPPUNIT_TEST( testOr );
	CPPUNIT_TEST( testXor );
	CPPUNIT_TEST( testInPlaceAnd );
	CPPUNIT_TEST( testInPlaceOr );
	CPPUNIT_TEST( testInPlaceXor );
	CPPUNIT_TEST_SUITE_END();
	
  public:
	void setUp()
	{
		qm1[0] = 0x00000000;
		qm1[1] = 0x00000000;
		qm1[2] = 0xffffffff;
		qm1[3] = 0xffffffff;
		
		qm2[0] = 0x00000000;
		qm2[1] = 0xffffffff;
		qm2[2] = 0x00000000;
		qm2[3] = 0xffffffff;
	}
	
	void tearDown() {}
	
	void testAnd()
	{
		qmask And = qm1 & qm2;
		CPPUNIT_ASSERT( And[0] == (qm1[0] & qm2[0]) );
		CPPUNIT_ASSERT( And[1] == (qm1[1] & qm2[1]) );
		CPPUNIT_ASSERT( And[2] == (qm1[2] & qm2[2]) );
		CPPUNIT_ASSERT( And[3] == (qm1[3] & qm2[3]) );
	}
	
	void testOr()
	{
		qmask Or = qm1 | qm2;
		CPPUNIT_ASSERT( Or[0] == (qm1[0] | qm2[0]) );
		CPPUNIT_ASSERT( Or[1] == (qm1[1] | qm2[1]) );
		CPPUNIT_ASSERT( Or[2] == (qm1[2] | qm2[2]) );
		CPPUNIT_ASSERT( Or[3] == (qm1[3] | qm2[3]) );
	}
	
	void testXor()
	{
		qmask Xor = qm1 ^ qm2;
		CPPUNIT_ASSERT( Xor[0] == (qm1[0] ^ qm2[0]) );
		CPPUNIT_ASSERT( Xor[1] == (qm1[1] ^ qm2[1]) );
		CPPUNIT_ASSERT( Xor[2] == (qm1[2] ^ qm2[2]) );
		CPPUNIT_ASSERT( Xor[3] == (qm1[3] ^ qm2[3]) );
	}
	
	void testInPlaceAnd()
	{
		qmask And(qm1);
		And &= qm2;
		CPPUNIT_ASSERT( And[0] == (qm1[0] & qm2[0]) );
		CPPUNIT_ASSERT( And[1] == (qm1[1] & qm2[1]) );
		CPPUNIT_ASSERT( And[2] == (qm1[2] & qm2[2]) );
		CPPUNIT_ASSERT( And[3] == (qm1[3] & qm2[3]) );
	}
	
	void testInPlaceOr()
	{
		qmask Or(qm1);
		Or |= qm2;
		CPPUNIT_ASSERT( Or[0] == (qm1[0] | qm2[0]) );
		CPPUNIT_ASSERT( Or[1] == (qm1[1] | qm2[1]) );
		CPPUNIT_ASSERT( Or[2] == (qm1[2] | qm2[2]) );
		CPPUNIT_ASSERT( Or[3] == (qm1[3] | qm2[3]) );
	}
	
	void testInPlaceXor()
	{
		qmask Xor(qm1);
		Xor ^= qm2;
		CPPUNIT_ASSERT( Xor[0] == (qm1[0] ^ qm2[0]) );
		CPPUNIT_ASSERT( Xor[1] == (qm1[1] ^ qm2[1]) );
		CPPUNIT_ASSERT( Xor[2] == (qm1[2] ^ qm2[2]) );
		CPPUNIT_ASSERT( Xor[3] == (qm1[3] ^ qm2[3]) );
	}
	
  private:
	qmask qm1, qm2;
};

#endif
