#include <mi4/VolumeDataCreator.hpp>
#include <mi4/marching_cubes.hpp>
#include <fstream>
int main ( int argc, char** argv )
{
        mi4::VolumeData<char> volume ( mi4::VolumeInfo ( mi4::Point3i ( 128, 128, 128 ) ) );
        mi4::VolumeDataCreator<char> creator ( volume );
        creator.setValue ( 1 );
        //creator.fillBlock( mi4::Point3i(10,10,10), mi4::Point3i(30, 30, 30));
        creator.fillSphere ( mi4::Point3i ( 63, 63, 63 ), 20 );
        std::ofstream fout ( "sphere.raw", std::ios::binary );
        volume.write ( fout );

	// 


        return 0;
}
