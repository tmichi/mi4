#include "mi4/Test.hpp"
#include "mi4/Xml.hpp"

class XmlTest : public mi4::TestCase {
public:
        explicit XmlTest (void) : mi4::TestCase("XmlTest")
        {
                return;
        }

        void init (void)
        {
                this->add(XmlTest::test0000ctor);
                return;
        }

        static void test0000ctor (void)
        {
                mi4::XmlDocument document("test");
                ASSERT_EQUALS(std::string("test"), document.getRoot().getName());

                return;
        }
};

static XmlTest test;









