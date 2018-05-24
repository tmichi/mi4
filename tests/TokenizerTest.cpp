#include <mi4/Test.hpp>
#include <mi4/Tokenizer.hpp>

class TokenizerTest : public mi4::TestCase
{
public:
        explicit TokenizerTest ( void  ) : mi4::TestCase ( "tokenizer_test" )
        {
                return;
        }

        void init ( void )
        {
                this->add ( TokenizerTest::test_basic0 ) ;
                this->add ( TokenizerTest::test_basic1 ) ;
                this->add ( TokenizerTest::test_delimiter0 ) ;
                this->add ( TokenizerTest::test_delimiter1 ) ;
                this->add ( TokenizerTest::test_fuzzy_compare ) ;
                return ;
        }

        static void test_basic0 ( void )
        {
                std::string inStr ( "a bb cc dd" );
                mi4::Tokenizer tokenizer ( inStr, " " );
                ASSERT_EQUALS ( 4, static_cast<int> ( tokenizer.size() ) );
                ASSERT_EQUALS ( std::string ( "a" ), tokenizer.get ( 0 ) );
                ASSERT_EQUALS ( std::string ( "bb" ), tokenizer.get ( 1 ) );
                ASSERT_EQUALS ( std::string ( "cc" ), tokenizer.get ( 2 ) );
                ASSERT_EQUALS ( std::string ( "dd" ), tokenizer.get ( 3 ) );
                return ;
        }

        static void test_basic1 ( void )
        {
                // starting with space
                std::string inStr ( " a b c d" );
                mi4::Tokenizer tokenizer ( inStr, " " );
                ASSERT_EQUALS ( 4, static_cast<int> ( tokenizer.size() ) );
                ASSERT_EQUALS ( std::string ( "a" ), tokenizer.get ( 0 ) );
                ASSERT_EQUALS ( std::string ( "b" ), tokenizer.get ( 1 ) );
                ASSERT_EQUALS ( std::string ( "c" ), tokenizer.get ( 2 ) );
                ASSERT_EQUALS ( std::string ( "d" ), tokenizer.get ( 3 ) );
                return ;
        }

        static void test_delimiter0 ( void )
        {
                std::string inStr ( "a bb;cc dd" );
                mi4::Tokenizer tokenizer ( inStr, ";" );
                ASSERT_EQUALS ( 2, static_cast<int> ( tokenizer.size() ) );
                ASSERT_EQUALS ( std::string ( "a bb" ), tokenizer.get ( 0 ) );
                ASSERT_EQUALS ( std::string ( "cc dd" ), tokenizer.get ( 1 ) );

                return;
        }

        static void test_delimiter1 ( void )
        {
                std::string inStr ( "a bb;cc dd" );
                mi4::Tokenizer tokenizer ( inStr, "; " );
                ASSERT_EQUALS ( 4, static_cast<int> ( tokenizer.size() ) );
                ASSERT_EQUALS ( std::string ( "a" ), tokenizer.get ( 0 ) );
                ASSERT_EQUALS ( std::string ( "bb" ), tokenizer.get ( 1 ) );
                ASSERT_EQUALS ( std::string ( "cc" ), tokenizer.get ( 2 ) );
                ASSERT_EQUALS ( std::string ( "dd" ), tokenizer.get ( 3 ) );

                return;
        }

        static void test_fuzzy_compare ( void )
        {
                ASSERT_EQUALS ( 0, mi4::fuzzy_compare ( "test ply", "test  ply  " ) );

                return;
        }
};

static TokenizerTest test;
