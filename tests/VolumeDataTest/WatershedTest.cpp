#include "mi4/Test.hpp"
#include "mi4/VolumeData.hpp"
//#include "mi4/WatershedProcessor.hpp"
class WatershedTest : public mi4::TestCase
{
public:
        explicit WatershedTest ( void  ) : mi4::TestCase ( "watershed_test" )
        {
                return;
        }

        void init ( void )
        {
                this->add ( WatershedTest::test_watershed) ;
                return ;
        }

        static void test_watershed( void )
        {
                mi4::VolumeData<int> data;
                ASSERT_EQUALS ( static_cast<int> ( 1), 1);
                return ;
        }
};
static WatershedTest test;
