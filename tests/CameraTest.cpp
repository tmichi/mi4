#include "mi4/Test.hpp"
#include "mi4/Camera.hpp"

class CameraTest : public mi4::TestCase
{
public:
        explicit CameraTest(void) : mi4::TestCase("camera_test")
        {
                return;
        }
        void init ( void )
        {
                this->add ( CameraTest::test_basic ) ;
                return ;
        }

        static void test_basic ( void )
        {
                ASSERT_EQUALS (1, 1);
                return ;
        }
};
static CameraTest test;
