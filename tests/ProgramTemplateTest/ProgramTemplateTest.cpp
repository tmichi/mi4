#ifndef PROGRAM_TEMPLATE_TEST_HPP
#define PROGRAM_TEMPLATE_TEST_HPP
#include "mi4/Test.hpp"
#include "mi4/ProgramTemplate.hpp"

class ProgramTemplateTest : public mi4::TestCase
{
public:
        explicit ProgramTemplateTest ( void  ) : mi4::TestCase ( "program_templte_test" )
        {
//		mi4::TestSuite::getInstance().add(this);
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
#endif
