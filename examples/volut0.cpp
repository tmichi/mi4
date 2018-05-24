#include <mi4/VolumeDataUtility.hpp>
#include <mi4/VolumeDataCreator.hpp>
#include <mi4/Timer.hpp>
#include <iostream>
int main ( int argc, char** argv )
{
        mi4::Timer timer ( "distance" );
        mi4::VolumeData<char> binary ( mi4::Point3i ( 512, 512, 512 ) );
        mi4::VolumeDataCreator<char> creator ( binary, 1 );
        creator.setValue ( 0 );
        creator.fillSphere ( mi4::Point3i ( 120, 120, 120 ), 50 );
        mi4::VolumeData<mi4::Vector3s> dvf = mi4::VolumeDataUtility::distance_field ( binary );
        auto df = mi4::VolumeDataUtility::vec2dist ( dvf );
        std::ofstream fout ( "result.raw" );
        df.write ( fout );
        return 0;
}
