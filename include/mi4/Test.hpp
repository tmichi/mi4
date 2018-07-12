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
#include <regex>

namespace mi4
{
        class TestCase;

        static void AddTestCase (TestCase *testcase);

#define ASSERT_EQUALS(expectedValue, actualValue)                  if(!TestCase::checkEquals( __FILE__, __LINE__, expectedValue, actualValue))return
#define ASSERT_EPSILON_EQUALS_DEFAULT(expectedValue, actualValue)  if(!TestCase::checkEpsilonEquals( __FILE__, __LINE__, expectedValue, actualValue))return
#define ASSERT_EPSILON_EQUALS(expectedValue, actualValue, epsilon) if(!TestCase::checkEpsilonEquals( __FILE__, __LINE__, expectedValue, actualValue, epsilon))return

        /**
         * @brief Basic class for writing test cases.
         * @sa TestSuite
         */
        class TestCase
        {
        private:
                TestCase (const TestCase&) = delete;
                void operator = (const TestCase&) = delete;
                TestCase (TestCase&&) = delete;
                void operator = (TestCase&&) = delete;
        public:
                explicit TestCase (const std::string& testName) : _testName(testName)
                {
                        mi4::AddTestCase ( this );
                }

                virtual ~TestCase ( void ) = default;

                void add (void ( *test ) (void))
                {
                        this->_tests.push_back ( test );
                }

                void run ( void )
                {
                        this->init();

                        for ( auto&& iter : this->_tests ) {
                                const auto numError = TestCase::getNumErrors();
                                ( *iter )();
                                std::cerr << ((numError == TestCase::getNumErrors()) ? "." : "!");
                        }
                        std::cerr << std::endl;
                        this->term();
                        return;
                }

                std::string getTestName ( void ) const
                {
                        return this->_testName;
                }

                static size_t getNumErrors (void)
                {
                        return TestCase::get_messages().size();
                }

                static void print ( std::ostream& out )
                {
                        for ( auto&& iter : TestCase::get_messages()) {
                                out << iter << std::endl;
                        }
                }
        protected:
                virtual void init ( void ) = 0;
                virtual void term ( void )
                {
                        return;
                }

                template < typename T >
                static bool checkEquals (const std::string& fileName, const int lineNo, const T expectedValue, const T actualValue)
                {
                        return (expectedValue == actualValue) ? true : TestCase::add_error_message(fileName, lineNo, expectedValue, actualValue);
                }

                template < typename T >
                static bool checkEpsilonEquals (const std::string& fileName, const int lineNo, const T expectedValue, const T actualValue, const T epsilon = T(1.0e-9))
                {
                        return (std::fabs(static_cast<double> ( expectedValue - actualValue )) <= epsilon) ? true : TestCase::add_error_message(fileName, lineNo, expectedValue, actualValue, epsilon, false);
                }

        private:
                template < typename T >
                static bool add_error_message (const std::string& fileName, const int lineNo, const T expectedValue, const T actualValue, const T epsilon = T(), const bool isExact = true)
                {
                        std::stringstream ss;
                        ss << fileName << ":" << lineNo << ": error. expected value = <" << expectedValue << ">" << ", actual value = <" << actualValue << ">";

                        if ( !isExact ) {
                                ss << ", epsilon=<" << epsilon << ">";
                        }

                        TestCase::get_messages().push_back(ss.str());
                        return false;
                }

                static std::list< std::string >& get_messages (void)
                {
                        static std::list< std::string > message;
                        return message;
                }
        private:
                std::string _testName;
                std::list< void (*) (void) > _tests;
        };

        /**
         * @class TestSuite "TestSuite.hpp"  "mi/TestSuite.hpp"
         * @brief Test suite class.
         * @sa TestCase
         */
        class TestSuite
        {
        private:

                TestSuite (const TestSuite&) = delete;
                void operator = ( const TestSuite& ) = delete;
                TestSuite (TestSuite&&) = delete;
                void operator = ( TestSuite&& ) = delete;
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
                        for ( auto&& iter : this->_testcases ) {
                                std::cerr << iter->getTestName() << ": ";
                                iter->run();
                        }

                        std::cerr << std::endl;

                        if ( TestCase::getNumErrors() == 0 ) {
                                std::cerr << "done." << std::endl;
                                return EXIT_SUCCESS;
                        } else {
                                const std::string fileName = testname + "-" + std::regex_replace(__DATE__, std::regex(R"( +)"), "-") + "-" + std::regex_replace(__TIME__, std::regex(R"(:+)"), "-") + ".log";
                                std::ofstream fout ( fileName.c_str() );

                                if ( !fout ) {
                                        std::cerr << "File cannot be open." << std::endl;
                                } else {
                                        TestCase::print(fout);
                                        std::cerr << "Errors are found. see " << fileName << "." << std::endl;
                                }

                                return EXIT_FAILURE;
                        }
                }
        private:
                std::list< TestCase * > _testcases;  ///< A set of test cases.
        };

        void AddTestCase ( TestCase* testcase )
        {
                mi4::TestSuite::getInstance().add ( testcase );
                return;
        }
}
#endif //MI_TEST_HPP
