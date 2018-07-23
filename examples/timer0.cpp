#include <mi4/Timer.hpp>

int main ( int argc, char** argv )
{
        mi4::Timer timer ( "test" );
        int c = 0;
        for ( int i = 0 ; i < 200000000 ; ++i ) {
                c += i ;
        }
	
        return 0;
}
