#include <mi4/Tokenizer.hpp>
#include <iostream>
int main ( int argc, char** argv )
{
        mi4::Tokenizer token0 ( " abc  cde f g" );
        mi4::Tokenizer token1 ( "abc cde f g" );

        if ( token0.toString().compare ( token1.toString() ) != 0 ) {
                std::cerr << "Error" << std::endl;
                return -1;
        }

        return 0;
}
