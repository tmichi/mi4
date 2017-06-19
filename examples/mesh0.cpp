#include <mi4/VolumeDataUtility.hpp>
#include <mi4/VolumeDataCreator.hpp>
#include <mi4/MeshUtility.hpp>
#include <mi4/VolumeDataPolygonizer.hpp>
#include <mi4/Timer.hpp>
#include <iostream>
int main ( int argc, char** argv )
{
	mi4::Timer timer ("distance");
        mi4::VolumeData<char> binary ( mi4::Point3i ( 512, 512, 512 ) );
        mi4::VolumeDataCreator<char> creator ( binary, 0 );
        creator.setValue ( 2 );
        creator.fillSphere ( mi4::Point3i ( 120, 120, 120 ), 50 );
	mi4::Mesh mesh = mi4::VolumeDataPolygonizer<char>(binary).polygonize( 1 );
	mi4::MeshUtility::savePly(mesh, "out.ply");
	std::cerr<<mesh.getNumVertices()<<std::endl;

        return 0;
}
