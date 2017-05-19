#include <mi4/ConnectedComponentLabeller.hpp>
#include <mi4/VolumeData.hpp>
#include <random>

int main ( int argc, char** argv )
{
        mi4::VolumeData<char> volume ( mi4::VolumeInfo ( mi4::Point3i ( 1024, 1024, 1024 ) ) );

        std::random_device rd;
        std::mt19937 gen ( rd() );
        std::uniform_int_distribution<> dis ( 0, 1023 );

        mi4::Range range ( mi4::Point3i ( -5, -5, -5 ), mi4::Point3i ( 5, 5, 5 ) );

        for ( int i = 1 ; i < 100; ++i ) {
                const mi4::Point3i p ( dis ( gen ), dis ( gen ), dis ( gen ) );
                std::cerr << p.transpose() << std::endl;

                for ( const auto& dp : range ) {
                        if ( !volume.getInfo().isValid ( p + dp ) ) continue;
                        volume.set ( p + dp, 1 );
                }
        }

        mi4::ConnectedComponentLabeller ccl ( volume );
        const auto nlabel = ccl.label ();
        std::cerr << "#nlabel:" << nlabel << std::endl;
        return 0;
}
