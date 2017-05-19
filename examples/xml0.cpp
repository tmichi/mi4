#include <mi4/Xml.hpp>
#include <iostream>

int main ( int argc, char** argv )
{
        mi4::XmlDocument doc ( "top" );
        mi4::XmlElement& elem = doc.getRoot();
        elem.addAttribute ( "attr", 1 );
        elem.addChildElement ( "child" );
        elem.addChildElement ( "child" );
        elem.addChildElement ( "child2" );
        elem.addChildText ( "text" );
        std::cout << doc.toString() << std::endl;



        for ( auto iter = elem.getChildren().begin(); iter != elem.getChildren().end() ; ++iter ) {
                if ( ( *iter )->getType() != mi4::ElementNode ) {
                        continue;
                }

                std::cerr << dynamic_cast<mi4::XmlElement&> ( **iter ).getName() << std::endl;;
        }

        return 0;
}
