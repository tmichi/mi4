#include "mi4/Test.hpp"
#include "mi4/ProgramTemplate.hpp"

class ArgumentTest : public mi4::TestCase
{
public:
        explicit ArgumentTest ( void  ) : mi4::TestCase ( "argument_test" )
        {
                return;
        }
        void init ( void )
        {
                this->add ( ArgumentTest::test_basic ) ;
                return ;
        }

        static void test_basic ( void )
        {
                ASSERT_EQUALS ( 0, 1 );
                return ;
        }
};
static ArgumentTest test;
