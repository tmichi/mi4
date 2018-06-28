#ifndef MI4_VOLUME_DATA_UTILITY_HPP
#define MI4_VOLUME_DATA_UTILITY_HPP 1
#include "ParallelFor.hpp"
#include "VolumeData.hpp"
#include "PriorityQueue.hpp"
#include <thread>
#include <string>
#include <cstring>
#include <iostream>
#include <fstream>
#include <cmath>
#include <cstdlib>
#include <vector>

namespace mi4
{
        class VolumeDataUtility
        {
        public:
                static bool& isDebugMode ( void )
                {
                        static bool isDebugMode = false;
                        return isDebugMode;
                }

                static void setDebugModeOn ( void )
                {
                        VolumeDataUtility::isDebugMode() = true;
                        return;
                }

                template< typename T>
                static bool open ( VolumeData<T>& data, const std::string& filename, const int headerSize = 0 )
                {
                        std::ifstream fin ( filename.c_str(), std::ios::binary );
                        if ( !fin ) {
                                std::cerr << "Open failed." << std::endl;
                                return false;
                        }
                        fin.seekg ( headerSize );
                        return data.read(fin);
                }
                template< typename T>
                static bool save ( VolumeData<T>& data, const std::string& filename )
                {
                        std::ofstream fout ( filename.c_str(), std::ios::binary );
                        if ( !fout ) {
                                std::cerr << filename << "cannot be open." << std::endl;
                                return false;
                        }
                        return data.write(fout);
                }

                template< typename T>
                static bool debug_save ( VolumeData<T>& data, const std::string& filename )
                {
                        if (VolumeDataUtility::isDebugMode()) {
                                std::cerr << "[debug] the result was saved to " << filename << std::endl;
                                return VolumeDataUtility::save(data, filename);
                        }
                        return true;
                }

                template <typename T>
                static VolumeData<T> changeEndian ( const VolumeData<T>& data )
                {
                        VolumeData<T> result ( data.getInfo() );
                        for ( const auto& p : Range ( data.getInfo() ) ) {
                                const T v = data.get ( p );
                                const int size = sizeof ( T );

                                if ( size > 1 ) {
                                        unsigned char* c = new unsigned char[size];
                                        std::memcpy ( &v, c, size );
                                        for ( int i = 0 ; i < size / 2 ; ++i ) {
                                                unsigned char tmp = c[i];
                                                c[i] = c[ size - 1 - i ];
                                                c[ size - 1 - i ] = tmp;
                                        }
                                        std::memcpy ( c, &v, size );
                                        delete[] c;
                                }

                                result.set ( p, v );
                        }

                        return std::move ( result );
                }

                /****
                 * Binarization
                 */
                template <typename T, typename S>
                static VolumeData<S> binarize ( const VolumeData<T>& data, const T threshold, const S fgValue = 1 )
                {
                        VolumeData<S> result ( data.getInfo() );

                        for ( const auto& p : Range ( data.getInfo() ) ) {
                                const S value = ( data.get ( p ) <  threshold ) ? 0 : fgValue;
                                result.set ( p, value );
                        }

                        return std::move ( result );
                }

                static VolumeData<char> extractBoundaryVoxels ( const VolumeData<char>& data )
                {
                        const char BG_VALUE = 0;
                        VolumeData <char> result ( data.getInfo() );
                        Range nbr ( mi4::Point3i ( -1, -1, -1 ), mi4::Point3i ( 1, 1, 1 ) );

                        for ( const auto p : Range ( data.getInfo() ) ) {
                                if ( data.get ( p ) == BG_VALUE ) {
                                        continue;
                                }

                                for ( const auto& d : nbr ) {
                                        if ( result.get ( p ) != BG_VALUE ) {
                                                break;
                                        }

                                        if ( d == mi4::Point3i ( 0, 0, 0 ) ) {
                                                continue;
                                        }

                                        const auto np = p + d;

                                        if ( !data.getInfo().isValid ( np ) ) {
                                                continue;
                                        }

                                        if ( data.get ( np ) == BG_VALUE ) {
                                                result.set ( p, 1 );
                                        }
                                }
                        }

                        return result;
                }
                //
                // 型変換
                //
                template <typename T, typename S>
                static VolumeData<S> cast ( const VolumeData<T>& data )
                {
                        VolumeData<S> result ( data.getInfo() );

                        for ( const auto& p : Range ( data.getInfo() ) ) {
                                result.set ( p, static_cast<S> ( data.get ( p ) ) );
                        }

                        return std::move ( result );
                }

                template <typename T>
                static VolumeData<T> clip ( const VolumeData<T>& data, const Point3i& bmin, const Point3i& bmax )
                {
                        const auto& info = data.getInfo();
                        const auto& pitch = info.getPitch();
                        mi4::Point3i size = bmax - bmin + Point3i ( 1, 1, 1 );
                        mi4::Point3d origin = info.getPointInSpace ( bmin );

                        VolumeData<T> result ( VolumeInfo ( size, pitch, origin ) );

                        for ( const auto& p : Range ( result.getInfo() ) ) {
                                result.set ( p, data.get ( bmin + p ) );
                        }

                        return std::move ( result );
                }


                static
                VolumeData<char> diff ( const VolumeData<char>& srcData, const VolumeData<char>& trgData )
                {
                        const auto& info =  srcData.getInfo();
                        VolumeData<char> outData ( info );

                        for ( const auto& p : mi4::Range ( info ) ) {
                                char value = 0;

                                if ( srcData.get ( p ) == 1  && trgData.get ( p ) == 0 ) {
                                        value = 1;
                                }

                                outData.set ( p, value );
                        }

                        return outData;
                }

                template< typename T>
                static VolumeData<T> negate_binary ( const VolumeData<T>& inData )
                {
                        const auto& info = inData.getInfo();
                        VolumeData<T> outData ( info );

                        for ( const auto& p : mi4::Range ( info ) ) {
                                outData.set ( p, 1 - inData.get ( p ) ) ;
                        }

                        return outData;
                }

                class morphology
                {
                private:
                        const VolumeData<char>& _inData;
                        VolumeData<char>& _outData;
                        const double _radius;
                        char _bg;
                        char _fg;

                public:
                        morphology ( const VolumeData<char>& inData, VolumeData<char>& outData, const double r, const bool isDilate = true ) : _inData ( inData ), _outData ( outData ), _radius ( r ), _bg ( 0 ), _fg ( 1 )
                        {
                                if ( !isDilate ) {
                                        this->_bg = 1;
                                        this->_fg = 0;
                                }

                                return;
                        }

                        void operator () ( const Point3i& p )
                        {
                                this->_outData.set ( p, this->get_value ( p ) ) ;
                                return;
                        }
                private:
                        inline char get_value ( const Point3i& p ) const
                        {
                                const auto& bg = this->_bg;
                                const auto& fg = this->_fg;

                                if ( this->_inData.get ( p ) == fg ) {
                                        return fg;
                                }

                                const auto& info = this->_inData.getInfo();
                                const auto& r = this->_radius;
                                const auto rp = info.convertVectorCeil ( mi4::Vector3d ( r, r, r ) );

                                for ( const auto& d : Range ( -rp, rp ) ) {
                                        if ( r * r <  info.getLengthSquared ( d ) ) {
                                                continue;
                                        }

                                        const auto np = p + d;

                                        if ( !info.isValid ( np ) ) {
                                                continue;
                                        }

                                        if ( this->_inData.get ( np ) == fg ) {
                                                return fg;
                                        }
                                }

                                return bg;
                        }
                };

                template<typename T>
                static VolumeData<T> erode ( const VolumeData<T>& inData, const double r )
                {
                        const auto& info = inData.getInfo();
                        VolumeData<T> result ( info );
                        Range range ( info );
                        const int nthreads = std::thread::hardware_concurrency();
                        mi4::parallel_for_each ( range.begin(), range.end(), morphology ( inData, result, r, false ), nthreads );
                        //std::for_each ( range.begin(), range.end(), morphology ( inData, result, r, false ) );
                        return std::move ( result );
                }

                template<typename T>
                static VolumeData<T> dilate ( const VolumeData<T>& inData, const double r )
                {
                        const auto& info = inData.getInfo();
                        VolumeData<T> result ( info );
                        Range range ( info );
                        const int nthreads = std::thread::hardware_concurrency();
                        mi4::parallel_for_each ( range.begin(), range.end(), morphology ( inData, result, r, true ), nthreads );
                        //std::for_each ( range.begin(), range.end(), morphology ( inData, result, r, true ) );
                        return std::move ( result );
                }

                // Convert vector distance field to scalar vector field
                static VolumeData<float> vec2dist ( const VolumeData<Vector3s>& input )
                {
                        const auto& info = input.getInfo();
                        VolumeData<float> result ( info );

                        for ( const auto& p : mi4::Range ( info ) ) {
                                result.set ( p, info.getLength ( input.get ( p ) ) );
                        }

                        return result;
                }

        private:
                static void dist_main ( const int z,  const VolumeData<char>& binary, const VolumeData<short>& tmpz, VolumeData<mi4::Vector3s>& result )
                {
                        const auto& info = binary.getInfo();
                        const auto& size = info.getSize();

                        for ( int y = 0 ; y < size.y() ; ++y ) {
                                std::vector<short> cloy ( size.x(), std::numeric_limits<short>::min() ) ;

                                for ( int x = 0 ; x < size.x() ; ++x ) {
                                        if ( binary.get ( x, y, z ) == 0 ) {
                                                cloy[x] = y;
                                        } else {
                                                float mind = std::numeric_limits<float>::max();

                                                for ( int dy = 0 ; dy < size.y() ; ++dy ) {
                                                        auto clz = tmpz.get ( mi4::Point3i ( x, dy, z ) );

                                                        if ( clz <  0 ) {
                                                                continue;        // when closest site does not exist.
                                                        }

                                                        Vector3s v ( 0, dy - y, clz - z ); // (0, dy, dz)
                                                        float d = info.getLength ( v );	 //dy * dy + dz * dz

                                                        if ( d < mind ) {
                                                                mind = d;
                                                                cloy[x] = dy;
                                                        }
                                                }
                                        }
                                }

                                for ( int x = 0 ; x < size.x() ; ++x ) {
                                        short clox = std::numeric_limits<short>::min();

                                        if ( binary.get ( x, y, z ) == 0 ) {
                                                clox = x;
                                        } else {
                                                float mind = std::numeric_limits<float>::max();

                                                for ( int dx = 0 ; dx < size.x() ; ++dx ) {
                                                        if ( cloy[dx] < 0 ) {
                                                                continue;
                                                        }

                                                        Vector3s v ( dx - x, cloy[dx] - y, tmpz.get ( mi4::Point3i ( dx, cloy[dx], z ) ) - z );
                                                        float d = info.getLength ( v );

                                                        if ( d < mind ) {
                                                                mind = d;
                                                                clox = dx;
                                                        }
                                                }
                                        }

                                        const short cx = clox;
                                        const short cy = cloy[cx];
                                        const short cz = tmpz.get ( mi4::Point3i ( cx, cy, z ) );
                                        result.set ( mi4::Point3i ( x, y, z ), Vector3s ( cx - x, cy - y, cz - z ) );
                                }
                        }

                        return;
                }

                static void dist_init ( const int x,  const VolumeData<char>& binary, VolumeData<short>& tmpz )
                {
                        const auto& info = binary.getInfo();
                        const auto& size = info.getSize();

                        for ( int y = 0 ; y < size.y() ; ++y ) {
                                int cz = std::numeric_limits<short>::min();

                                for ( int z = 0 ; z < size.z() ; ++z ) {
                                        const mi4::Point3i p ( x, y, z );

                                        if ( binary.get ( p ) == 0 ) {
                                                cz = z;
                                        }

                                        tmpz.set ( p, cz );
                                }

                                cz = std::numeric_limits<short>::min();

                                for ( int z = size.z() - 1 ; z >= 0 ; --z ) {
                                        const mi4::Point3i p ( x, y, z );

                                        if ( binary.get ( p ) == 0 ) {
                                                cz = z;
                                        } else {
                                                if ( std::fabs ( cz - z ) > std::fabs ( z - tmpz.get ( p ) ) ) {
                                                        cz = tmpz.get ( p );
                                                }
                                        }

                                        tmpz.set ( p, cz );
                                }
                        }
                }

                static std::vector<int> getRanges ( const size_t length, const int nthread )
                {
                        std::vector<int> ranges;
                        const int npt = ( length  - 1 ) / nthread + 1;
                        int n = 0;

                        while ( n < length ) {
                                ranges.push_back ( n );
                                n += npt;
                        }

                        ranges.push_back ( length );
                        return ranges;
                }

        public:
                static VolumeData<Vector3s> distance_field ( const VolumeData<char>& binary )
                {
                        const int nthreads = std::thread::hardware_concurrency();
                        const auto& info = binary.getInfo();
                        const auto& size = info.getSize();
                        mi4::VolumeData<Vector3s> result ( info );
                        mi4::VolumeData<short> tmpz ( info );

                        std::vector<std::thread> threads;

                        for ( int x = 0 ; x < size.x() ; ++x ) {
                                threads.push_back ( std::thread ( &VolumeDataUtility::dist_init, x, std::ref ( binary ), std::ref ( tmpz ) ) );

                                if ( threads.size() == nthreads ) {
                                        for ( auto& th : threads ) {
                                                th.join();
                                        }

                                        threads.clear();
                                }
                        }

                        for ( auto& th : threads ) {
                                th.join();
                        }

                        threads.clear();

                        for ( int z = 0 ; z < size.z() ; ++z ) {
                                threads.push_back ( std::thread ( &VolumeDataUtility::dist_main, z, std::ref ( binary ), std::ref ( tmpz ), std::ref ( result ) ) );

                                if ( threads.size() == nthreads ) {
                                        for ( auto& th : threads ) {
                                                th.join();
                                        }

                                        threads.clear();
                                }
                        }

                        for ( auto& th : threads ) {
                                th.join();
                        }

                        return result;
                }

                /********
                 * Watershed.
                 * valid label: >0
                 * weight : distance
                 ********/
                template <typename T>
                static
                void watershed ( mi4::VolumeData<T>& label,  const mi4::VolumeData<float>& weight )
                {
                        const auto& info = label.getInfo();

                        PriorityQueue <Point3i> pq;

                        // initialization. set pixels to the queue
                        for ( const auto& p : mi4::Range ( info ) ) {
                                if ( label.get ( p ) > 0 && weight.get ( p ) > 0 ) {
                                        pq.push ( p, -weight.get ( p ) ) ;
                                }
                        }

                        std::vector<Point3i> nbr = { Point3i ( 1, 0, 0 ), Point3i ( -1, 0, 0 ), Point3i ( 0, 1,  0 ), Point3i ( 0, -1, 0 ), Point3i ( 0, 0, 1 ), Point3i ( 0, 0, -1 ) };

                        while ( !pq.empty() ) {
                                const auto p = pq.getTopIndex();
                                pq.pop();
                                const int labelId = label.get ( p ); // Label ID to be propagated.

                                for ( const auto& d : nbr ) {
                                        const Point3i np = d + p;

                                        if ( ! info.isValid ( np ) ) {
                                                continue;
                                        }

                                        if ( label.get ( np ) != 0 ) {
                                                continue;
                                        }

                                        if ( weight.get ( np ) > 0 ) {
                                                label.set ( np, labelId );
                                                pq.push ( np, -weight.get ( np ) );
                                        }
                                }
                        }

                        return;
                }

        private:
        };
}

#endif //MI4_VOLUME_DATA_UTILITY_HPP
