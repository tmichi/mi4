//
// Created by Takashi Michikawa on 2018/07/06.
//
#include <mi4/Test.hpp>
#include <mi4/VolumeDataUtility.hpp>

class VolumeDataUtilityTest : public mi4::TestCase {
public:
        explicit VolumeDataUtilityTest (void) : mi4::TestCase("VolumeDataUtilityTest")
        {
                return;
        }

        void init (void)
        {
                this->add(VolumeDataUtilityTest::test_default_constructor);
                return;
        }

        static void test_default_constructor (void)
        {
                mi4::VolumeData< int > data;
                ASSERT_EQUALS (static_cast<int> ( data.isReadable()), static_cast<int> ( true ));
                return;
        }
};

static VolumeDataUtilityTest test;
