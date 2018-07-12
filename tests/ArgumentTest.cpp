#include "mi4/Test.hpp"
#include "mi4/ProgramTemplate.hpp"
class ArgumentTest : public mi4::TestCase
{
public:
        explicit ArgumentTest ( void  ) : mi4::TestCase ( "argument_test" )
        {
                this->add(ArgumentTest::test00);
                this->add(ArgumentTest::test01);
                this->add(ArgumentTest::test02);
                this->add(ArgumentTest::test03);
                return;
        }
        void init ( void )
        {

                return ;
        }

        static void test00 (void)
        {
                // 0 個
                mi4::Argument arg;
                ASSERT_EQUALS (0, arg.size());
                return;
        }

        static void test01 (void)
        {
                // 1個
                const int argc = 1;
                const char *argv[] = {"a.out"};
                mi4::Argument arg(argc, const_cast<char **>(argv));
                ASSERT_EQUALS (1, arg.size());
                ASSERT_EQUALS (std::string("a.out"), arg.get< std::string >(size_t(0)));
                return;
        }

        static void test02 (void)
        {
                // 3個
                const int argc = 3;
                const char *argv[] = {"a.out", "a", "b"};
                mi4::Argument arg(argc, const_cast<char **>(argv));
                ASSERT_EQUALS (3, arg.size());
                ASSERT_EQUALS (std::string("a.out"), arg.get< std::string >(size_t(0)));
                ASSERT_EQUALS (std::string("a"), arg.get< std::string >(size_t(1)));
                ASSERT_EQUALS (std::string("b"), arg.get< std::string >(size_t(2)));
                return ;
        }
        static void test03 (void)
        {
                const int argc = 3;
                const char *argv[] = {"a.out", "-a", "10"};
                mi4::Argument arg(argc, const_cast<char **>(argv));
                ASSERT_EQUALS (3, arg.size());
                ASSERT_EQUALS (std::string("a.out"), arg.get< std::string >(0));
                ASSERT_EQUALS (static_cast<int>(10), arg.get< int >("-a", 1));
                ASSERT_EQUALS (static_cast<uint8_t>(10), arg.get< uint8_t >("-a", 1));
                ASSERT_EQUALS (static_cast<uint16_t>(10), arg.get< uint16_t >("-a", 1));
                ASSERT_EQUALS (static_cast<uint32_t>(10), arg.get< uint32_t >("-a", 1));
                ASSERT_EQUALS (static_cast<uint64_t>(10), arg.get< uint64_t >("-a", 1));
                ASSERT_EQUALS (static_cast<int8_t>(10), arg.get< int8_t >("-a", 1));
                ASSERT_EQUALS (static_cast<int16_t>(10), arg.get< int16_t >("-a", 1));
                ASSERT_EQUALS (static_cast<int32_t>(10), arg.get< int32_t >("-a", 1));
                ASSERT_EQUALS (static_cast<int64_t>(10), arg.get< int64_t >("-a", 1));
                ASSERT_EPSILON_EQUALS (static_cast<float>(10.0), arg.get< float >("-a", 1), static_cast<float>(1.0e-10));
                ASSERT_EPSILON_EQUALS (static_cast<double>(10.0), arg.get< double >("-a", 1), 1.0e-10);
                ASSERT_EQUALS (std::string("10"), arg.get< std::string >("-a", 1));

                // failed
                ASSERT_EQUALS (int64_t(), arg.get< int64_t >("-a", 2));
                ASSERT_EQUALS (uint8_t(), arg.get< uint8_t >("-a", 2));
                ASSERT_EQUALS (uint16_t(), arg.get< uint16_t >("-a", 2));
                ASSERT_EQUALS (uint32_t(), arg.get< uint32_t >("-a", 2));
                ASSERT_EQUALS (uint64_t(), arg.get< uint64_t >("-a", 2));
                ASSERT_EQUALS (int8_t(), arg.get< int8_t >("-a", 2));
                ASSERT_EQUALS (int16_t(), arg.get< int16_t >("-a", 2));
                ASSERT_EQUALS (int32_t(), arg.get< int32_t >("-a", 2));
                ASSERT_EQUALS (int64_t(), arg.get< int64_t >("-a", 2));
                ASSERT_EPSILON_EQUALS (float(), arg.get< float >("-a", 2), static_cast<float>(1.0e-10));
                ASSERT_EPSILON_EQUALS (double(), arg.get< double >("-a", 2), 1.0e-10);
                ASSERT_EQUALS (std::string(), arg.get< std::string >("-a", 2));
                return;
        }
};
static ArgumentTest test;
