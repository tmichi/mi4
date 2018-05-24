#include <mi4/VolumeData.hpp>
#include <iostream>
int main ( int argc, char** argv )
{
        mi4::VolumeData<unsigned char> volume ( mi4::Point3i ( 100, 100, 100 ) );
        volume.at ( 10, 10, 10 ) = 10;
        std::cerr << ( int )volume.at ( 10, 10, 10 ) << std::endl;

        mi4::Range range ( mi4::Point3i ( 5, 5, 5 ), mi4::Point3i ( 8, 9, 10 ) );

        for ( auto&& iter : range ) {
                std::cerr << iter.transpose() << std::endl;
        }

        return 0;
}
