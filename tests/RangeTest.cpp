//
// Created by Takashi Michikawa on 2018/07/04.
//

#include "mi4/Test.hpp"
#include "mi4/VolumeData.hpp"

class RangeTest : public mi4::TestCase {
public:
        explicit RangeTest (void) : mi4::TestCase("range_test")
        {
                return;
        }

        void init (void)
        {
                this->add(RangeTest::test_default_constructor);
                this->add(RangeTest::test01);

                return;
        }

        static void test_default_constructor (void)
        {
                mi4::Range range;
                const auto bmin = range.getMin();
                const auto bmax = range.getMax();
                ASSERT_EQUALS(0, bmin.x());
                ASSERT_EQUALS(0, bmin.y());
                ASSERT_EQUALS(0, bmin.z());
                ASSERT_EQUALS(0, bmax.x());
                ASSERT_EQUALS(0, bmax.y());
                ASSERT_EQUALS(0, bmax.z());
                return;
        }

        static void test01 (void)
        {
                mi4::Range range(mi4::Point3i(10, 10, 10), mi4::Point3i(99, 99, 99));
                const auto bmin = range.getMin();
                const auto bmax = range.getMax();
                ASSERT_EQUALS(10, bmin.x());
                ASSERT_EQUALS(10, bmin.y());
                ASSERT_EQUALS(10, bmin.z());
                ASSERT_EQUALS(99, bmax.x());
                ASSERT_EQUALS(99, bmax.y());
                ASSERT_EQUALS(99, bmax.z());

                return;
        }
};

static RangeTest rangetest;