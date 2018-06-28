#include "mi4/Test.hpp"
#include "mi4/ProgramTemplate.hpp"
class ArgumentTest : public mi4::TestCase
{
public:
        explicit ArgumentTest ( void  ) : mi4::TestCase ( "argument_test" )
        {
                return;
        }
        void init ( void )
        {
                this->add ( ArgumentTest::test_basic ) ;
                this->add(ArgumentTest::test_basic01);
                this->add(ArgumentTest::test_basic02);
                return ;
        }

        static void test_basic ( void )
        {
                // 0 個
                mi4::Argument arg;
                ASSERT_EQUALS (0, arg.size());
                return;
        }

        static void test_basic01(void) {
                // 1個
                const int argc = 1;
                const char *argv[] = {"a.out"};
                mi4::Argument arg(argc, const_cast<char **>(argv));
                ASSERT_EQUALS (1, arg.size());
                ASSERT_EQUALS (std::string("a.out"), arg.get< std::string >(size_t(0)));
                return;
        }

        static void test_basic02(void) {
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
};
static ArgumentTest test;
