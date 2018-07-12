#include "mi4/Test.hpp"
#include "mi4/ProgramTemplate.hpp"

class ProgramTemplateTest : public mi4::TestCase
{
public:
        explicit ProgramTemplateTest (void) : mi4::TestCase("ProgramTemplte_test")
        {
                return;
        }
        void init ( void )
        {
                this->add ( ProgramTemplateTest::test_basic ) ;
                return ;
        }

        static void test_basic ( void )
        {
                ASSERT_EQUALS ( 0, 0 );
                return ;
        }
};
static ProgramTemplateTest test;

