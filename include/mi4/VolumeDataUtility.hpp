#ifndef MI4_VOLUME_DATA_UTILITY_HPP
#define MI4_VOLUME_DATA_UTILITY_HPP 1
#include <mi4/VolumeData.hpp>
#include <string>
#include <iostream>
#include <fstream>

namespace mi4
{
        class VolumeDataUtility
        {
        private:

        public:

                template <typename T>
                static VolumeData<T> binarize ( const VolumeData<T>& data, const T threshold )
                {
                        VolumeData<T> result ( data.getInfo() );

                        for ( const auto& p : Range ( data.getInfo() ) ) {
                                const T value = ( data.get ( p ) <  threshold ) ? 0 : 1;
                                result.set ( p, value );
                        }

                        return std::move ( result );
                }

                static VolumeData<char> extractBoundaryVoxels ( const VolumeData<char>& data )
                {
                        VolumeData <char> result ( data.getInfo() );
                        Range nbr ( mi4::Point3i ( -1, -1, -1 ), mi4::Point3i ( 1, 1, 1 ) );

                        for ( const auto p : Range ( data.getInfo() ) ) {
                                if ( data.get ( p ) == 0 ) continue;
                                for ( const auto& d : nbr ) {
                                        if ( result.get ( p ) != 0 ) break;
                                        if ( d == mi4::Point3i ( 0, 0, 0 ) ) continue;

                                        const auto np = p + d;

                                        if ( !data.getInfo().isValid ( np ) )  continue;
                                        if ( data.get ( np ) == 0 ) result.set ( p, 1 );
                                }
                        }
                        //return std::move (result);
                        return result;
                }

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
                        VolumeData<T> result ( VolumeInfo ( bmax - bmin + Point3i ( 1, 1, 1 ) ) );

                        for ( const auto& p : Range ( result.getInfo() ) ) {
                                result.set ( p, data.get ( bmin + p ) );
                        }

                        return std::move ( result );
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
                static VolumeData<T> mmerode ( const VolumeData<T>& inData, const double r )
                {
                        const auto& info = inData.getInfo();
                        VolumeData<T> result ( info );
                        Range range ( info );
                        //mi::parallel_for_each ( range.begin(), range.end(), mi::erode ( inData, outData, r)   );
                        std::for_each ( range.begin(), range.end(), morphology ( inData, result, r, false ) );
                        return std::move ( result );
                }

                template<typename T>
                static VolumeData<T> mmdilate ( const VolumeData<T>& inData, const double r )
                {
                        const auto& info = inData.getInfo();
                        VolumeData<T> result ( info );
                        Range range ( info );
                        //mi::parallel_for_each ( range.begin(), range.end(), mi::erode ( inData, outData, r)   );
                        std::for_each ( range.begin(), range.end(), morphology ( inData, result, r, true ) );
                        return std::move ( result );
                }
/*
                static VolumeData<mi4::Vector3s> distance_field ( const VolumeData<char>& binaryData) {
                        const auto& info = inData.getInfo();
                        VolumeData<mi4::Vector3s> resultData (info);

                        return std::move ( resultData);
                }
                */
        };
}
#endif //MI4_VOLUME_DATA_UTILITY_HPP
