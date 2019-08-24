#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>

#include "SIMDFloatTest.h"
#include "SIMDMaskTest.h"
#include "SIMDQuadTest.h"

CPPUNIT_TEST_SUITE_REGISTRATION( SIMDFloatTest );
CPPUNIT_TEST_SUITE_REGISTRATION( SIMDMaskTest );
CPPUNIT_TEST_SUITE_REGISTRATION( SIMDQuadTest );

int main( int argc, char **argv)
{
	CppUnit::TextUi::TestRunner runner;
	CppUnit::TestFactoryRegistry &registry = CppUnit::TestFactoryRegistry::getRegistry();
	
	runner.addTest( registry.makeTest() );
	runner.run();
}
