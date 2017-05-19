#include "mi4/Test.hpp"
#include "mi4/VolumeData.hpp"

class VolumeInfoTest : public mi4::TestCase
{
public:
        explicit VolumeInfoTest ( void  ) : mi4::TestCase ( "volume_info_test" )
        {
//		mi4::TestSuite::getInstance().add(this);
                return;
        }
        void init ( void )
        {
                this->add ( VolumeInfoTest::test_default_constructor0 ) ;
                this->add ( VolumeInfoTest::test_default_constructor1 ) ;
                return ;
        }

        static void test_default_constructor0 ( void )
        {
                mi4::VolumeInfo info;
                ASSERT_EQUALS ( 0, info.getSize().x() );
                ASSERT_EQUALS ( 0, info.getSize().y() );
                ASSERT_EQUALS ( 0, info.getSize().z() );

                ASSERT_EPSILON_EQUALS_DEFAULT ( 1.0, info.getPitch().x() );
                ASSERT_EPSILON_EQUALS_DEFAULT ( 1.0, info.getPitch().y() );
                ASSERT_EPSILON_EQUALS_DEFAULT ( 1.0, info.getPitch().z() );

                ASSERT_EPSILON_EQUALS_DEFAULT ( 0.0, info.getOrigin().x() );
                ASSERT_EPSILON_EQUALS_DEFAULT ( 0.0, info.getOrigin().y() );
                ASSERT_EPSILON_EQUALS_DEFAULT ( 0.0, info.getOrigin().z() );
                return ;
        }

        static void test_default_constructor1 ( void )
        {
                mi4::VolumeInfo info ( mi4::Point3i ( 100, 200, 300 ), mi4::Point3d ( 0.4, 0.4, 0.5 ), mi4::Point3d ( 10, 20, 30 ) );

                ASSERT_EQUALS ( 100, info.getSize().x() );
                ASSERT_EQUALS ( 200, info.getSize().y() );
                ASSERT_EQUALS ( 300, info.getSize().z() );

                ASSERT_EPSILON_EQUALS_DEFAULT ( 0.4, info.getPitch().x() );
                ASSERT_EPSILON_EQUALS_DEFAULT ( 0.4, info.getPitch().y() );
                ASSERT_EPSILON_EQUALS_DEFAULT ( 0.5, info.getPitch().z() );

                ASSERT_EPSILON_EQUALS_DEFAULT ( 10.0, info.getOrigin().x() );
                ASSERT_EPSILON_EQUALS_DEFAULT ( 20.0, info.getOrigin().y() );
                ASSERT_EPSILON_EQUALS_DEFAULT ( 30.0, info.getOrigin().z() );
                return ;
        }

};
static VolumeInfoTest test;

