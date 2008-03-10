#ifndef __COUNTINGTRACKER_H
#define __COUNTINGTRACKER_H

//
// The CountingTracker helps writing TestListener's by
// taking care of the "dirty" work.
// See YesNoRunner and ErrorPrinter for examples.
//

#include <cxxtest/TestListener.h>
#include <cxxtest/TestTracker.h>

namespace CxxTest
{
    class CountingTracker : public TestListener, public TestTracker
    {
    public:
	void setListener( TestListener *l ) { _l = TestListener::fixPointer(l); }

	const TestDescription &test() const { return *_test; }
	const SuiteDescription &suite() const { return *_suite; }
	const WorldDescription &world() const { return *_world; }

        unsigned failedTests() const { return _failedTests; }
	unsigned testFailedAsserts() const { return _testFailedAsserts; }
        unsigned suiteFailedTests() const { return _suiteFailedTests; }
        unsigned failedSuites() const { return _failedSuites; }

	void enterWorld( const WorldDescription &wd )
        {
	    setWorld( &wd );
            _failedTests = _testFailedAsserts = _suiteFailedTests = _failedSuites = 0;
            _l->enterWorld(wd);
        }

        void enterSuite( const SuiteDescription &sd )
        {
	    setSuite( &sd );
            _testFailedAsserts = _suiteFailedTests = 0;
            _l->enterSuite(sd);
        }
        
	void enterTest( const TestDescription &td )
        {
	    setTest( &td );
            _testFailedAsserts = false;
            _l->enterTest(td);
        }

        void leaveTest( const TestDescription &td )
        {
            _l->leaveTest(td);
	    setTest();
        }

	void leaveSuite( const SuiteDescription &sd )
	{
	    _l->leaveSuite(sd);
	    setSuite();
	}

	void leaveWorld( const WorldDescription &wd )
	{
	    _l->leaveWorld(wd);
	}

        void warning( const char *file, unsigned line, const char *expression )
        {
            _l->warning( file, line, expression );
        }

        void failedTest( const char *file, unsigned line, const char *expression )
        {
            countFailure();
            _l->failedTest( file, line, expression );
        }
        
	void failedAssert( const char *file, unsigned line, const char *expression )
        {
            countFailure();
            _l->failedAssert( file, line, expression );
        }

	void failedAssertEquals( const char *file, unsigned line,
                                 const char *xStr, const char *yStr,
                                 const char *x, const char *y )
        {
            countFailure();
            _l->failedAssertEquals( file, line, xStr, yStr, x, y );
        }

	void failedAssertDelta( const char *file, unsigned line,
                                const char *xStr, const char *yStr, const char *dStr,
                                const char *x, const char *y, const char *d )
        {
            countFailure();
            _l->failedAssertDelta( file, line, xStr, yStr, dStr, x, y, d );
        }
	void failedAssertDiffers( const char *file, unsigned line,
                                  const char *xStr, const char *yStr,
                                  const char *value )
        {
            countFailure();
            _l->failedAssertDiffers( file, line, xStr, yStr, value );
        }
        
	void failedAssertLessThan( const char *file, unsigned line,
				   const char *xStr, const char *yStr,
				   const char *x, const char *y )
        {
            countFailure();
            _l->failedAssertLessThan( file, line, xStr, yStr, x, y );
        }

	void failedAssertThrows( const char *file, unsigned line,
                                 const char *expression, const char *type,
                                 bool otherThrown )
        {
            countFailure();
            _l->failedAssertThrows( file, line, expression, type, otherThrown );
        }
        
	void failedAssertThrowsNot( const char *file, unsigned line, const char *expression )
        {
            countFailure();
            _l->failedAssertThrowsNot( file, line, expression );
        }
        
    private:
        unsigned _failedTests, _testFailedAsserts, _suiteFailedTests, _failedSuites;
	TestListener *_l;
	const WorldDescription *_world;
	const SuiteDescription *_suite;
	const TestDescription *_test;

	void setWorld( const WorldDescription *w = 0 )
	{
	    _world = w ? w : &(WorldDescription::_dummy);
	    setSuite();
	}

	void setSuite( const SuiteDescription *s = 0 )
	{
	    _suite = s ? s : &(SuiteDescription::_dummy);
	    setTest();
	}

	void setTest( const TestDescription *t = 0 )
	{
	    _test = t ? t : &(TestDescription::_dummy);
	}

        void countFailure()
        {
            if ( ++ _testFailedAsserts == 1 ) {
                ++ _failedTests;
                if ( ++ _suiteFailedTests == 1 )
                    ++ _failedSuites;
            }
        }
    };
};


#endif // __COUNTINGTRACKER_H
