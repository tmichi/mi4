//
// Created by Takashi Michikawa on 2018/07/05.
//

#include "mi4/Test.hpp"
#include "mi4/VolumeData.hpp"

class VolumeInfoTest : public mi4::TestCase {
public:
        explicit VolumeInfoTest (void) : mi4::TestCase("VolumeInfoTest")
        {
                return;
        }

        void init (void)
        {
                this->add(VolumeInfoTest::test_default_constructor);
                this->add(VolumeInfoTest::test01);
                this->add(VolumeInfoTest::test02);
                return;
        }

        static void test_default_constructor (void)
        {
                mi4::VolumeInfo info;
                ASSERT_EQUALS (1, 1);
                return;
        }
        static void test01 (void)
        {
                mi4::VolumeInfo info(mi4::Point3i(300, 200, 100), mi4::Point3d(0.2, 0.3, 0.4), mi4::Point3d(0.1, 0.2, 0.3));

                auto size = info.getSize();
                ASSERT_EQUALS(mi4::Point3i(300, 200, 101).transpose(), size.transpose());

                auto pitch = info.getPitch();
                ASSERT_EPSILON_EQUALS_DEFAULT(0.2, pitch.x());
                ASSERT_EPSILON_EQUALS_DEFAULT(0.3, pitch.y());
                ASSERT_EPSILON_EQUALS_DEFAULT(0.4, pitch.z());

                auto origin = info.getOrigin();
                ASSERT_EPSILON_EQUALS_DEFAULT(0.1, origin.x());
                ASSERT_EPSILON_EQUALS_DEFAULT(0.2, origin.y());
                ASSERT_EPSILON_EQUALS_DEFAULT(0.3, origin.z());


                auto bmin = info.getMin();
                ASSERT_EQUALS(0, bmin.x());
                ASSERT_EQUALS(0, bmin.y());
                ASSERT_EQUALS(0, bmin.z());

                auto bmax = info.getMax();
                ASSERT_EQUALS(299, bmax.x());
                ASSERT_EQUALS(199, bmax.y());
                ASSERT_EQUALS(99, bmax.z());

                mi4::Point3i p0(50, 50, 50);
                auto p1 = info.getPointInSpace(p0);
                auto p2 = mi4::Point3d(p0.x() * pitch.x(), p0.y() * pitch.y(), p0.z() * pitch.z()) + origin;
                ASSERT_EQUALS(p2.x(), p1.x());
                ASSERT_EQUALS(p2.y(), p1.y());
                ASSERT_EQUALS(p2.z(), p1.z());

                mi4::Point3i p3(10, 20, 30);
                const Eigen::Vector3d p4(p3.x() * pitch.x(), p3.y() * pitch.y(), p3.z() * pitch.z());
                const auto p5 = info.getVector(p3);
                ASSERT_EPSILON_EQUALS_DEFAULT(p4.x(), p5.x());
                ASSERT_EPSILON_EQUALS_DEFAULT(p4.y(), p5.y());
                ASSERT_EPSILON_EQUALS_DEFAULT(p4.z(), p5.z());

                const mi4::Point3d p6(50.3, 30.1, 60.99);
                const auto p7 = info.getPointInVoxel(p6);
                ASSERT_EQUALS(250, p7.x());
                ASSERT_EQUALS(99, p7.y());
                ASSERT_EQUALS(151, p7.z());

                const auto p8 = info.getPointInVoxelFloor(p6);
                ASSERT_EQUALS(250, p8.x());
                ASSERT_EQUALS(99, p8.y());
                ASSERT_EQUALS(151, p8.z());

                const auto p9 = info.getPointInVoxelCeil(p6);
                ASSERT_EQUALS(251, p9.x());
                ASSERT_EQUALS(100, p9.y());
                ASSERT_EQUALS(152, p9.z());


        }

        static void test02 (void)
        {
                mi4::VolumeInfo info(mi4::Point3i(100, 100, 100), mi4::Point3d(1, 1, 1), mi4::Point3d(0, 0, 0));

                auto info1 = info.clip(mi4::Point3d(10, 10, 10), mi4::Point3d(20, 20, 20));
                std::cerr << info1.toStringInfo() << std::endl;
        }
};

static VolumeInfoTest test;

