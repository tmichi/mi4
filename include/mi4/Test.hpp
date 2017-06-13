/**
 * @file Test.hpp
 * @author Takashi Michikawa <michikawa@acm.org>
 */
#ifndef MI_TEST_HPP
#define MI_TEST_HPP 1
#include <fstream>
#include <iostream>
#include <sstream>

#include <cmath>
#include <list>
#include <string>
#include <memory>

namespace mi4
{
        class TestCase;
        static void AddTestCase ( TestCase* testcase ) ;
#define ASSERT_EQUALS(expectedValue, actualValue)			\
        		if(!TestCase::checkEquals( __FILE__, __LINE__, expectedValue, actualValue))return
#define ASSERT_EPSILON_EQUALS_DEFAULT(expectedValue, actualValue)	\
        		if(!TestCase::checkEpsilonEquals( __FILE__, __LINE__, expectedValue, actualValue))return
#define ASSERT_EPSILON_EQUALS(expectedValue, actualValue, epsilon)	\
        		if(!TestCase::checkEpsilonEquals( __FILE__, __LINE__, expectedValue, actualValue, epsilon))return


        /**
         * @brief Basic class for writing test cases.
         * @sa TestSuite
         */
        class TestCase
        {
        private:
                TestCase ( const TestCase& );
                void operator = ( const TestCase& );
        public:
                explicit TestCase ( const std::string& testName = std::string ( "test" ) ) : _testName ( testName )
                {
                        mi4::AddTestCase ( this );
                        return;
                }

                virtual ~TestCase ( void ) = default;

                void add ( void ( * test ) ( void ) )
                {
                        this->_tests.push_back ( test );
                        return;
                }

                void run ( void )
                {
                        this->init();

                        for ( auto && iter : this->_tests ) {
                                const int numError = TestCase::getNumErrors();

                                ( *iter )();

                                if ( numError == TestCase::getNumErrors() ) {
                                        std::cerr << ".";
                                } else {
                                        std::cerr << "*";
                                }
                        }

                        std::cerr << std::endl;
                        this->term();
                        return;
                }

                std::string getTestName ( void ) const
                {
                        return this->_testName;
                }

                static int getNumErrors ( void )
                {
                        return static_cast<int> ( TestCase::get_message().size() );
                }

                static void print ( std::ostream& out )
                {
                        std::list<std::string>& message = TestCase::get_message();

                        for ( auto && iter : message ) {
                                out << iter  << std::endl;
                        }

                        return;
                }
        protected:
                virtual void init ( void ) = 0;
                virtual void term ( void )
                {
                        return;
                }

                template< typename T>
                static bool checkEquals ( const char* fileName, const int lineNo,  const T expectedValue, const T actualValue )
                {
                        if ( expectedValue == actualValue ) {
                                return true;
                        }

                        TestCase::add_message ( fileName, lineNo, expectedValue, actualValue );
                        return false;
                }

                template <typename T>
                static bool checkEpsilonEquals ( const char* fileName, const int lineNo,  const T expectedValue, const T actualValue, const T epsilon = T ( 1.0e-9 ) )
                {
                        if ( std::fabs ( static_cast<double> ( expectedValue - actualValue ) ) < epsilon ) {
                                return true;
                        }

                        TestCase::add_message ( fileName, lineNo, expectedValue, actualValue, epsilon, false );
                        return false;
                }

        private:
                template< typename T>
                static void add_message ( const char* fileName, const int lineNo, const T expectedValue, const T actualValue, const T epsilon = T(), const bool isExact = true )
                {
                        std::stringstream ss;
                        ss << fileName << ":" << lineNo << ": error. expected value = <" << expectedValue << ">" << ", actual value = <" << actualValue << ">";

                        if ( !isExact ) {
                                ss << ", epsilon=<" << epsilon << ">";
                        }

                        TestCase::get_message().push_back ( ss.str() );
                }

                static std::list<std::string>& get_message ( void )
                {
                        static std::list< std::string > message;
                        return message;
                }
        private:
                std::string _testName;
                std::list<void ( * ) ( void )> _tests;
        };

        /**
         * @class TestSuite "TestSuite.hpp"  "mi/TestSuite.hpp"
         * @brief Test suite class.
         * @sa TestCase
         */
        class TestSuite
        {
        private:

                TestSuite       ( const TestSuite& );
                void operator = ( const TestSuite& );
                TestSuite       ( TestSuite&& );
                void operator = ( TestSuite&& );

                TestSuite ( void ) = default;
                ~TestSuite ( void ) = default;
        public:
                /**
                 * @brief Get instance.
                 * @return Instance.
                 */
                static TestSuite& getInstance ( void )
                {
                        static TestSuite instance;
                        return instance;
                }

                /**
                 * @brief Add test case.
                 */
                void add ( TestCase* testcase )
                {
                        this->_testcases.push_back ( testcase );
                }

                /**
                 * @brief Run test cases.
                 */
                int run ( const std::string& testname )
                {
                        for ( auto && iter : this->_testcases ) {
                                std::cerr << iter->getTestName() << ": ";
                                iter->run();
                        }

                        std::cerr << std::endl;

                        if ( TestCase::getNumErrors() == 0 ) {
                                return EXIT_SUCCESS;
                        } else {
                                std::string fileName ( testname );
                                fileName.append ( TestSuite::replace_str ( __DATE__ ) ).append ( TestSuite::replace_str ( __TIME__ ) ).append ( ".log" );
                                std::ofstream fout ( fileName.c_str() );
                                TestCase::print ( fout );

                                std::cerr << "error(s) found. see " << fileName << "." << std::endl;
                                return EXIT_FAILURE;
                        }
                }
        private:
                static std::string replace_str ( const std::string& inStr )
                {
                        std::string str = inStr;
                        const std::string rep_str ( "-" );

                        for ( auto pos = str.find ( " " ) ; pos != std::string::npos ; pos = str.find ( " ", 1 + pos ) ) {
                                str.replace ( pos, 1, rep_str );
                        }

                        for ( auto pos = str.find ( ":" ) ; pos != std::string::npos ; pos = str.find ( ":", 1 + pos ) ) {
                                str.replace ( pos, 1, rep_str );
                        }

                        return str;
                }
        private:
                std::list< TestCase* > _testcases; ///< A set of test cases.
        };

        void AddTestCase ( TestCase* testcase )
        {
                mi4::TestSuite::getInstance().add ( testcase );
                return;
        }
}
#endif //MI_TEST_HPP
