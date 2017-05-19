#include <mi4/VolumeDataUtility.hpp>
#include <mi4/SystemInfo.hpp>
#include <random>
int main ( int argc, char** argv )
{
        int n = 1024;
        mi4::VolumeData<char> volume ( mi4::VolumeInfo ( mi4::Point3i ( n, n, n ) ) );

        std::random_device rd;
        std::mt19937 gen ( rd() );
        std::uniform_int_distribution<> dis ( 0, n-1);

        mi4::Range range ( mi4::Point3i ( -5, -5, -5 ), mi4::Point3i ( 5, 5, 5 ) );

        for ( int i = 1 ; i < 100; ++i ) {
                const mi4::Point3i p ( dis ( gen ), dis ( gen ), dis ( gen ) );

                for ( const auto& dp : range ) {
                        if ( !volume.getInfo().isValid ( p + dp ) ) continue;
                        volume.set ( p + dp, 1 );
                }
        }
        //mi4::VolumeData<int> castData = mi4::VolumeDataUtility::cast<char, int>(volume);
        mi4::VolumeData<int> castData = mi4::VolumeDataUtility::cast<char, int>(volume);
        std::cerr<<mi4::SystemInfo::getPeakMemorySize()/1024.0/1024.0<<std::endl;


        return 0;
}