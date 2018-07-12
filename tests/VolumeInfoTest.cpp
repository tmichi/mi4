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
                return;
        }

        static void test_default_constructor (void)
        {
                mi4::VolumeInfo info;
                ASSERT_EQUALS (1, 1);
                return;
        }
};

static VolumeInfoTest test;

