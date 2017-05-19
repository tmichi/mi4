#include <mi4/VolumeDataCreator.hpp>
int main ( int argc, char** argv ) {
        mi4::VolumeData<int> volume(mi4::VolumeInfo(mi4::Point3i(128, 128, 128)));
        mi4::VolumeDataCreator<int> creator(volume);
        creator.fillBlock( mi4::Point3i(10,10,10), mi4::Point3i(30, 30, 30));

        return 0;
}
