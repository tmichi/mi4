#include "mi4/Test.hpp"
#include "mi4/Projector.hpp"

class ProjectorTest : public mi4::TestCase {
public:
        explicit ProjectorTest (void) : mi4::TestCase("ProjectorTest")
        {
                return;
        }

        void init (void)
        {
                this->add(ProjectorTest::test00);
                this->add(ProjectorTest::test01);

                return;
        }

        static void test00 (void)
        {
                int vp[4] = {0, 0, 0, 0};
                mi4::Projector projector(Eigen::Matrix4d(), Eigen::Matrix4d(), {vp[0], vp[1], vp[2], vp[3]});
                return;
        }

        static void test01 (void)
        {
                return;
        }
};

static ProjectorTest projectortest;