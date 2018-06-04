#include "mi4/Test.hpp"
#include "mi4/ColorMapper.hpp"

class ColorMapperTest : public mi4::TestCase
{
public:
        explicit ColorMapperTest ( void  ) : mi4::TestCase ( "argument_test" )
        {
                return;
        }
        void init ( void )
        {
                this->add ( ColorMapperTest::test_basic ) ;
                return ;
        }

        static void test_basic ( void )
        {
                ASSERT_EQUALS ( 0, 1 );
                return ;
        }
};
static ColorMapperTest test;
