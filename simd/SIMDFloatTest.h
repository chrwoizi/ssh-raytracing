#ifndef SIMDFLOATTEST_H
#define SIMDFLOATTEST_H

#include <cmath>
#include <cppunit/extensions/HelperMacros.h>

#include "simd.h"


class SIMDFloatTest : public CppUnit::TestFixture, public SIMDmemAligned {

	CPPUNIT_TEST_SUITE( SIMDFloatTest );
	CPPUNIT_TEST( testAddition );
	CPPUNIT_TEST( testSubtraction );
	CPPUNIT_TEST( testMultiplication );
	CPPUNIT_TEST( testDivison );
	CPPUNIT_TEST( testInPlaceAddition );
	CPPUNIT_TEST( testInPlaceSubtraction );
	CPPUNIT_TEST( testInPlaceMultiplication );
	CPPUNIT_TEST( testInPlaceDivison );
	CPPUNIT_TEST( testComparisonEqual );
	CPPUNIT_TEST( testComparisonNotEqual );
	CPPUNIT_TEST( testComparisonLess );
	CPPUNIT_TEST( testComparisonLessOrEqual );
	CPPUNIT_TEST( testComparisonGreater );
	CPPUNIT_TEST( testComparisonGreaterOrEqual );
	CPPUNIT_TEST( testCondAssign );
	CPPUNIT_TEST( testMax );
	CPPUNIT_TEST( testMin );
	CPPUNIT_TEST( testAbs );
	CPPUNIT_TEST( testSqrt );
	CPPUNIT_TEST( testRcp );
	CPPUNIT_TEST_SUITE_END();
	
  public:
	void setUp()
	{
		qf1[0] = 0.0f;
		qf1[1] = 1.0f;
		qf1[2] = 7.5f;
		qf1[3] = -0.3777f;
		
		qf2[0] = 0.2f;
		qf2[1] = -1.625f;
		qf2[2] = 7.5f;
		qf2[3] = 0.666666f;
		
		qm[0] = 0x00000000;
		qm[1] = 0xffffffff;
		qm[2] = 0x00000000;
		qm[3] = 0xffffffff;
	}
	
	void tearDown() {}
	
	void testAddition()
	{
		qfloat sum = qf1 + qf2;
		float sum0 = qf1[0] + qf2[0];
		float sum1 = qf1[1] + qf2[1];
		float sum2 = qf1[2] + qf2[2];
		float sum3 = qf1[3] + qf2[3];
		CPPUNIT_ASSERT( sum[0] == sum0 );
		CPPUNIT_ASSERT( sum[1] == sum1 );
		CPPUNIT_ASSERT( sum[2] == sum2 );
		CPPUNIT_ASSERT( sum[3] == sum3 );
	}
	
	void testSubtraction()
	{
		qfloat difference = qf1 - qf2;
		float difference0 = qf1[0] - qf2[0];
		float difference1 = qf1[1] - qf2[1];
		float difference2 = qf1[2] - qf2[2];
		float difference3 = qf1[3] - qf2[3];
		CPPUNIT_ASSERT( difference[0] == difference0 );
		CPPUNIT_ASSERT( difference[1] == difference1 );
		CPPUNIT_ASSERT( difference[2] == difference2 );
		CPPUNIT_ASSERT( difference[3] == difference3 );
	}
	
	void testMultiplication()
	{
		qfloat product = qf1 * qf2;
		float product0 = qf1[0] * qf2[0];
		float product1 = qf1[1] * qf2[1];
		float product2 = qf1[2] * qf2[2];
		float product3 = qf1[3] * qf2[3];
		CPPUNIT_ASSERT( product[0] == product0 );
		CPPUNIT_ASSERT( product[1] == product1 );
		CPPUNIT_ASSERT( product[2] == product2 );
		CPPUNIT_ASSERT( product[3] == product3 );
	}
	
	void testDivison()
	{
		qfloat quotient = qf1 / qf2;
		float quotient0 = qf1[0] / qf2[0];
		float quotient1 = qf1[1] / qf2[1];
		float quotient2 = qf1[2] / qf2[2];
		float quotient3 = qf1[3] / qf2[3];
		CPPUNIT_ASSERT( quotient[0] == quotient0 );
		CPPUNIT_ASSERT( quotient[1] == quotient1 );
		CPPUNIT_ASSERT( quotient[2] == quotient2 );
		CPPUNIT_ASSERT( quotient[3] == quotient3 );
	}
	
	void testInPlaceAddition()
	{
		qfloat sum(qf2);
		sum += qf1;
		float sum0 = qf2[0] + qf1[0];
		float sum1 = qf2[1] + qf1[1];
		float sum2 = qf2[2] + qf1[2];
		float sum3 = qf2[3] + qf1[3];
		CPPUNIT_ASSERT( sum[0] == sum0 );
		CPPUNIT_ASSERT( sum[1] == sum1 );
		CPPUNIT_ASSERT( sum[2] == sum2 );
		CPPUNIT_ASSERT( sum[3] == sum3 );
	}
	
	void testInPlaceSubtraction()
	{
		qfloat difference(qf2);
		difference -= qf1;
		float difference0 = qf2[0] - qf1[0];
		float difference1 = qf2[1] - qf1[1];
		float difference2 = qf2[2] - qf1[2];
		float difference3 = qf2[3] - qf1[3];
		CPPUNIT_ASSERT( difference[0] == difference0 );
		CPPUNIT_ASSERT( difference[1] == difference1 );
		CPPUNIT_ASSERT( difference[2] == difference2 );
		CPPUNIT_ASSERT( difference[3] == difference3 );
	}
	
	void testInPlaceMultiplication()
	{
		qfloat product(qf2);
		product *= qf1;
		float product0 = qf2[0] * qf1[0];
		float product1 = qf2[1] * qf1[1];
		float product2 = qf2[2] * qf1[2];
		float product3 = qf2[3] * qf1[3];
		CPPUNIT_ASSERT( product[0] == product0 );
		CPPUNIT_ASSERT( product[1] == product1 );
		CPPUNIT_ASSERT( product[2] == product2 );
		CPPUNIT_ASSERT( product[3] == product3 );
	}
	
	void testInPlaceDivison()
	{
		qfloat quotient(qf2);
		quotient /= qf1;
		float quotient0 = qf2[0] / qf1[0];
		float quotient1 = qf2[1] / qf1[1];
		float quotient2 = qf2[2] / qf1[2];
		float quotient3 = qf2[3] / qf1[3];
		CPPUNIT_ASSERT( quotient[0] == quotient0 );
		CPPUNIT_ASSERT( quotient[1] == quotient1 );
		CPPUNIT_ASSERT( quotient[2] == quotient2 );
		CPPUNIT_ASSERT( quotient[3] == quotient3 );
	}
	
	void testComparisonEqual()
	{
		qmask comparison;
		comparison = qf1 == qf2;
		CPPUNIT_ASSERT( comparison[0] == (qf1[0] == qf2[0] ? 0xffffffff : 0x0) );
		CPPUNIT_ASSERT( comparison[1] == (qf1[1] == qf2[1] ? 0xffffffff : 0x0) );
		CPPUNIT_ASSERT( comparison[2] == (qf1[2] == qf2[2] ? 0xffffffff : 0x0) );
		CPPUNIT_ASSERT( comparison[3] == (qf1[3] == qf2[3] ? 0xffffffff : 0x0) );
	}
	
	void testComparisonNotEqual()
	{
		qmask comparison;
		comparison = qf1 != qf2;
		CPPUNIT_ASSERT( comparison[0] == (qf1[0] != qf2[0] ? 0xffffffff : 0x0) );
		CPPUNIT_ASSERT( comparison[1] == (qf1[1] != qf2[1] ? 0xffffffff : 0x0) );
		CPPUNIT_ASSERT( comparison[2] == (qf1[2] != qf2[2] ? 0xffffffff : 0x0) );
		CPPUNIT_ASSERT( comparison[3] == (qf1[3] != qf2[3] ? 0xffffffff : 0x0) );
	}
	
	void testComparisonLess()
	{
		qmask comparison;
		comparison = qf1 < qf2;
		CPPUNIT_ASSERT( comparison[0] == (qf1[0] < qf2[0] ? 0xffffffff : 0x0) );
		CPPUNIT_ASSERT( comparison[1] == (qf1[1] < qf2[1] ? 0xffffffff : 0x0) );
		CPPUNIT_ASSERT( comparison[2] == (qf1[2] < qf2[2] ? 0xffffffff : 0x0) );
		CPPUNIT_ASSERT( comparison[3] == (qf1[3] < qf2[3] ? 0xffffffff : 0x0) );
	}
	
	void testComparisonLessOrEqual()
	{
		qmask comparison;
		comparison = qf1 <= qf2;
		CPPUNIT_ASSERT( comparison[0] == (qf1[0] <= qf2[0] ? 0xffffffff : 0x0) );
		CPPUNIT_ASSERT( comparison[1] == (qf1[1] <= qf2[1] ? 0xffffffff : 0x0) );
		CPPUNIT_ASSERT( comparison[2] == (qf1[2] <= qf2[2] ? 0xffffffff : 0x0) );
		CPPUNIT_ASSERT( comparison[3] == (qf1[3] <= qf2[3] ? 0xffffffff : 0x0) );
	}
	
	void testComparisonGreater()
	{
		qmask comparison;
		comparison = qf1 > qf2;
		CPPUNIT_ASSERT( comparison[0] == (qf1[0] > qf2[0] ? 0xffffffff : 0x0) );
		CPPUNIT_ASSERT( comparison[1] == (qf1[1] > qf2[1] ? 0xffffffff : 0x0) );
		CPPUNIT_ASSERT( comparison[2] == (qf1[2] > qf2[2] ? 0xffffffff : 0x0) );
		CPPUNIT_ASSERT( comparison[3] == (qf1[3] > qf2[3] ? 0xffffffff : 0x0) );
	}
	
	void testComparisonGreaterOrEqual()
	{
		qmask comparison;
		comparison = qf1 >= qf2;
		CPPUNIT_ASSERT( comparison[0] == (qf1[0] >= qf2[0] ? 0xffffffff : 0x0) );
		CPPUNIT_ASSERT( comparison[1] == (qf1[1] >= qf2[1] ? 0xffffffff : 0x0) );
		CPPUNIT_ASSERT( comparison[2] == (qf1[2] >= qf2[2] ? 0xffffffff : 0x0) );
		CPPUNIT_ASSERT( comparison[3] == (qf1[3] >= qf2[3] ? 0xffffffff : 0x0) );
	}
	
	void testCondAssign()
	{
		qfloat result;
		result.condAssign(qm, qf1, qf2);
		CPPUNIT_ASSERT( result[0] == qf2[0] );
		CPPUNIT_ASSERT( result[1] == qf1[1] );
		CPPUNIT_ASSERT( result[2] == qf2[2] );
		CPPUNIT_ASSERT( result[3] == qf1[3] );
	}
	
	void testMax()
	{
		qfloat maximum;
		maximum = max(qf1, qf2);
		CPPUNIT_ASSERT( maximum[0] == std::max(qf1[0], qf2[0]) );
		CPPUNIT_ASSERT( maximum[1] == std::max(qf1[1], qf2[1]) );
		CPPUNIT_ASSERT( maximum[2] == std::max(qf1[2], qf2[2]) );
		CPPUNIT_ASSERT( maximum[3] == std::max(qf1[3], qf2[3]) );
	}
	
	void testMin()
	{
		qfloat minimum;
		minimum = min(qf1, qf2);
		CPPUNIT_ASSERT( minimum[0] == std::min(qf1[0], qf2[0]) );
		CPPUNIT_ASSERT( minimum[1] == std::min(qf1[1], qf2[1]) );
		CPPUNIT_ASSERT( minimum[2] == std::min(qf1[2], qf2[2]) );
		CPPUNIT_ASSERT( minimum[3] == std::min(qf1[3], qf2[3]) );
	}
	
	void testAbs()
	{
		qfloat absolut;
		absolut = abs(qf1);
		CPPUNIT_ASSERT( absolut[0] == fabsf(qf1[0]) );
		CPPUNIT_ASSERT( absolut[1] == fabsf(qf1[1]) );
		CPPUNIT_ASSERT( absolut[2] == fabsf(qf1[2]) );
		CPPUNIT_ASSERT( absolut[3] == fabsf(qf1[3]) );
	}
	
	void testSqrt()
	{
		qfloat squareroot;
		squareroot = sqrt(qf2);
		CPPUNIT_ASSERT_DOUBLES_EQUAL( squareroot[0], sqrtf(qf2[0]), 0.000001f);
		CPPUNIT_ASSERT( isnanf(squareroot[1]));	// sqrtf(-x) = NaN
		CPPUNIT_ASSERT_DOUBLES_EQUAL( squareroot[2], sqrtf(qf2[2]), 0.000001f);
		CPPUNIT_ASSERT_DOUBLES_EQUAL( squareroot[3], sqrtf(qf2[3]), 0.000001f);
	}
	
	void testRcp()
	{
		qfloat reciproc;
		reciproc =rcp(qf2);
		CPPUNIT_ASSERT_DOUBLES_EQUAL( reciproc[0], 1.0f/qf2[0], 0.00005f);
		CPPUNIT_ASSERT_DOUBLES_EQUAL( reciproc[1], 1.0f/qf2[1], 0.00005f);
		CPPUNIT_ASSERT_DOUBLES_EQUAL( reciproc[2], 1.0f/qf2[2], 0.00005f);
		CPPUNIT_ASSERT_DOUBLES_EQUAL( reciproc[3], 1.0f/qf2[3], 0.00005f);
	}
	
  private:
	qfloat qf1, qf2;
	qmask qm;
};

#endif
