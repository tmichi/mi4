#ifndef DISTANCE_FIELD_HPP
#define DISTANCE_FIELD_HPP 1
#include <mi4/VolumeData.hpp>
#include <thread>
#include <vector>

static mi4::VolumeData<float> vec2dist ( const mi4::VolumeData<mi4::Vector3s>& input )
{
        const auto& info = input.getInfo();
        mi4::VolumeData<float> result ( info );
        for ( const auto& p : mi4::Range ( info ) ) {
                result.set ( p, info.getLength ( input.get ( p ) ) );
        }

        return result;
}

static mi4::VolumeData<mi4::Vector3s> distance_field ( const mi4::VolumeData<char>& binary );

///----- Implementation
std::vector<int> get_ranges ( const int x, const int nthread )
{
        std::vector<int> result;
        const double r = x * 1.0 / nthread;
        for ( int i = 0; i < nthread ; ++i ) {
                result.push_back ( static_cast<int> ( r * i ) );
        }
        result.push_back ( x );
        return result;
}

void dist_init ( const int sx, const int ex,  const mi4::VolumeData<char>& binary, mi4::VolumeData<short>& tmpz )
{
        const auto& size = binary.getInfo().getSize();
        for ( auto x = sx ; x < ex ; ++x ) {
                for ( auto y = 0 ; y < size.y() ; ++y ) {
                        auto cz = std::numeric_limits<short>::min();
                        for ( auto z = 0 ; z < size.z() ; ++z ) {
                                const mi4::Point3i p ( x, y, z );
                                if ( binary.get ( p ) == 0 ) cz = static_cast<short>(z);
                                tmpz.set ( p, cz );
                        }

                        for ( int z = size.z() - 1 ; z >= 0 ; --z ) {
                                const mi4::Point3i p ( x, y, z );
                                if ( binary.get ( p ) == 0 ) {
                                        cz = static_cast<short>(z);
                                } else if ( std::abs ( cz - z ) > std::abs ( z - tmpz.get ( p ) ) ) {
					cz = tmpz.get ( p );
                                }
                                tmpz.set ( p, cz );
                        }
                }
        }
}

void dist_main ( const int sz, const int ez,  const mi4::VolumeData<char>& binary, const mi4::VolumeData<short>& tmpz, mi4::VolumeData<mi4::Vector3s>& result )
{
        const auto& info = binary.getInfo();
        const auto& size = info.getSize();

        for ( auto z = sz ; z < ez ; ++z ) {
                for ( auto y = 0 ; y < size.y() ; ++y ) {
                        std::vector<short> cloy ( size.x(), std::numeric_limits<short>::min() ) ;
                        for ( auto x = 0 ; x < size.x() ; ++x ) {
                                if ( binary.get ( x, y, z ) == 0 ) {
                                        cloy[x] = y;
                                } else {
                                        auto  mind = std::numeric_limits<float>::max();

                                        for ( auto dy = 0 ; dy < size.y() ; ++dy ) {
                                                auto clz = tmpz.get ( mi4::Point3i ( x, dy, z ) );
                                                if ( clz <  0 )  continue;        // when closest site does not exist.
                                                const mi4::Vector3s v ( 0, dy - y, clz - z ); // (0, dy, dz)
                                                const auto d = info.getLength ( v );	 //dy * dy + dz * dz

                                                if ( d < mind ) {
                                                        mind = d;
                                                        cloy[x] = dy;
                                                }
                                        }
                                }
                        }

                        for ( auto x = 0 ; x < size.x() ; ++x ) {
                                auto clox = std::numeric_limits<short>::min();

                                if ( binary.get ( x, y, z ) == 0 ) {
                                        clox = x;
                                } else {
                                        auto mind = std::numeric_limits<float>::max();

                                        for ( auto dx = 0 ; dx < size.x() ; ++dx ) {
                                                if ( cloy[dx] < 0 ) {
                                                        continue;
                                                }

                                                mi4::Vector3s v ( dx - x, cloy[dx] - y, tmpz.get ( mi4::Point3i ( dx, cloy[dx], z ) ) - z );
                                                const auto d = info.getLength ( v );

                                                if ( d < mind ) {
                                                        mind = d;
                                                        clox = dx;
                                                }
                                        }
                                }

                                const short cx = clox;
                                const short cy = cloy[cx];
                                const short cz = tmpz.get ( mi4::Point3i ( cx, cy, z ) );
                                result.set ( mi4::Point3i ( x, y, z ), mi4::Vector3s ( cx - x, cy - y, cz - z ) );
                        }
                }
        }

        return;
}

mi4::VolumeData<mi4::Vector3s> distance_field ( const mi4::VolumeData<char>& binary )
{
        const auto   nthreads = std::thread::hardware_concurrency();
        const auto& info = binary.getInfo();
        const auto& size = info.getSize();

        mi4::VolumeData<mi4::Vector3s> result ( info );
        mi4::VolumeData<short> tmpz ( info );

        std::vector<std::thread> threads;

        const auto range0 = get_ranges ( size.x(), nthreads );
        for ( int i = 0 ; i < nthreads ; ++i ) {
                threads.push_back ( std::thread ( &dist_init, range0[i], range0[i + 1], std::ref ( binary ), std::ref ( tmpz ) ) );
        }
        for ( auto& th : threads ) {
                th.join();
        }
        threads.clear();
        const auto range1 = get_ranges ( size.z(), nthreads );

        for ( int i = 0 ; i < nthreads ; ++i ) {
                threads.push_back ( std::thread ( &dist_main, range1[i], range1[i + 1], std::ref ( binary ), std::ref ( tmpz ), std::ref ( result ) ) );
        }
        for ( auto& th : threads ) {
                th.join();
        }
        return result;
}


#endif


