//
// Created by Takashi Michikawa on 2018/07/19.
//

#include "mi4/Type.hpp"
#include "mi4/Test.hpp"

class TypeTest : public mi4::TestCase {
public:
        explicit TypeTest (void) : mi4::TestCase("type_test")
        {
                return;
        }
        void init (void)
        {
                this->add(TypeTest::test0);
                this->add(TypeTest::test1);
                return;
        }

        static void test0 (void)
        {
                ASSERT_EQUALS(std::string("char"), mi4::get_type_string(typeid(char)));
                ASSERT_EQUALS(std::string("uchar"), mi4::get_type_string(typeid(unsigned char)));
                ASSERT_EQUALS(std::string("short"), mi4::get_type_string(typeid(short)));
                ASSERT_EQUALS(std::string("ushort"), mi4::get_type_string(typeid(unsigned short)));
                ASSERT_EQUALS(std::string("int"), mi4::get_type_string(typeid(int)));
                ASSERT_EQUALS(std::string("uint"), mi4::get_type_string(typeid(unsigned int)));
                ASSERT_EQUALS(std::string("float"), mi4::get_type_string(typeid(float)));
                ASSERT_EQUALS(std::string("double"), mi4::get_type_string(typeid(double)));

                ASSERT_EQUALS(std::string("char"), mi4::get_type_string(typeid(int8_t)));
                ASSERT_EQUALS(std::string("uchar"), mi4::get_type_string(typeid(uint8_t)));
                ASSERT_EQUALS(std::string("short"), mi4::get_type_string(typeid(int16_t)));
                ASSERT_EQUALS(std::string("ushort"), mi4::get_type_string(typeid(uint16_t)));
                ASSERT_EQUALS(std::string("int"), mi4::get_type_string(typeid(int32_t)));
                ASSERT_EQUALS(std::string("uint"), mi4::get_type_string(typeid(uint32_t)));
                ASSERT_EQUALS(std::string("float"), mi4::get_type_string(typeid(float_t)));
                ASSERT_EQUALS(std::string("double"), mi4::get_type_string(typeid(double_t)));
                return;
        }

        static void test1 (void)
        {
                ASSERT_EQUALS(std::string(), mi4::get_type_string(typeid(std::ofstream)));
                return;
        }

};

static TypeTest test;