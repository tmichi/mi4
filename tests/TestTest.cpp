#include "mi4/Test.hpp"
#include "mi4/Test.hpp"

class TestTest : public mi4::TestCase
{
public:
        explicit TestTest ( void  ) : mi4::TestCase ( "test_test" )
        {
                return;
        }
        void init ( void )
        {
                this->add ( TestTest::test_assert_equals ) ;
                this->add ( TestTest::test_assert_epsilon_equals ) ;
                return ;
        }

        static void test_assert_equals ( void )
        {
                TestTest::assert_equals_check<int>();
                TestTest::assert_equals_check<char>();
                TestTest::assert_equals_check<short>();
                return ;
        }

        static void test_assert_epsilon_equals ( void )
        {
                TestTest::assert_epsilon_equals_check<double>();
                TestTest::assert_epsilon_equals_check<float>();
                return ;
        }

        template <typename T>
        static void assert_equals_check ( void )
        {
                T a0 = 0;
                T b0 = 0;
                ASSERT_EQUALS ( a0, b0 );
        }
        template <typename T>
        static void assert_epsilon_equals_check ( void )
        {
                T a0 = T ( 0 );
                T b0 = T ( 0 );
                ASSERT_EPSILON_EQUALS_DEFAULT ( a0, b0 );

                T a1 = T ( 0 );
                T b1 = T ( 1.0e-10 );
                ASSERT_EPSILON_EQUALS_DEFAULT ( a1, b1 );

                T a2 = T ( 0 );
                T b2 = T ( -1.0e-10 );
                ASSERT_EPSILON_EQUALS_DEFAULT ( a2, b2 );
        }
};
static TestTest test;

