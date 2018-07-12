#include "mi4/Test.hpp"
#include "mi4/VolumeData.hpp"
class VolumeDataTest : public mi4::TestCase
{
public:
        explicit VolumeDataTest ( void  ) : mi4::TestCase ( "volume_data_test" )
        {
                return;
        }

        void init ( void )
        {
                this->add ( VolumeDataTest::test_default_constructor ) ;
                return ;
        }

        static void test_default_constructor ( void )
        {
                mi4::VolumeData<int> data;
                ASSERT_EQUALS (static_cast<int> ( data.isReadable()), static_cast<int> ( true ));
                return ;
        }
};
static VolumeDataTest test;
