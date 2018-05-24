#include <mi4/VolumeData.hpp>
#include <mi4/VolumeDataUtility.hpp>
#include <iostream>
int main ( int argc, char** argv )
{
        mi4::VolumeData<unsigned char> volume ( mi4::Point3i ( 100, 100, 100 ) );
        volume.at ( 10, 10, 10 ) = 10;
        std::cerr << ( int )volume.at ( 10, 10, 10 ) << std::endl;
        volume.save ( "test.vol" );
        mi4::VolumeData<unsigned char> volume1 ( volume.getInfo() );
        volume1.open ( "test.vol" );
        std::cerr << ( int )volume1.at ( 10, 10, 10 ) << std::endl;
        return 0;
}
