//#include <mi4/ConnectedComponentLabeller.hpp>
#include <mi4/ccl.hpp>
#include <mi4/VolumeData.hpp>
#include <mi4/VolumeDataCreator.hpp>
#include <random>
#include <fstream>
int main ( void ) 
{
        const int n = 64;
        mi4::VolumeData<char> volume ( mi4::VolumeInfo ( mi4::Point3i ( n, n, n ) ) );

        std::random_device rd;
        std::mt19937 gen ( rd() );
        std::uniform_int_distribution<> dis ( 0, n - 1 );

        mi4::VolumeDataCreator<char> creator ( volume );
        creator.setValue ( 1 );
        creator.fill();
        creator.setValue ( 0 );

        for ( int i = 0 ; i  < n ; i += 4 ) {
                creator.fillBlock ( mi4::Point3i ( 0, 0, i ), mi4::Point3i ( n - 2, n - 1, i ) );
                creator.fillBlock ( mi4::Point3i ( 1, 0, i + 2 ), mi4::Point3i ( n - 1, n - 1, i + 2 ) );
        }

        /*	for ( int i = 0 ; i  < n ; i += 4) {
        		creator.fillBlock ( mi4::Point3i(0,  i, 0) ,  mi4::Point3i(n-2, i   , n-1) );
        		creator.fillBlock ( mi4::Point3i(1, i+2, 0) , mi4::Point3i(n-1, i+2, n-1) );
        	}
        */

        auto result = mi4::ccl ( volume ).label ( true ).getData<short>();
        std::ofstream fout ( "label.raw", std::ios::binary );
        result.write ( fout );
        fout.close();

//        mi4::ConnectedComponentLabeller ccl ( volume );
//        const auto nlabel = ccl.label ();
//        std::cerr << "#nlabel:" << nlabel << std::endl;
        return 0;
}
