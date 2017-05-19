#include "mi4/Svg.hpp"
int main ( int argc, char** argv )
{
        mi4::Svg svg ( 100, 100 );
        svg.drawCircle ( 50, 50, 10 );
        std::cout << svg.toString() << std::endl;
        return 0;
}
