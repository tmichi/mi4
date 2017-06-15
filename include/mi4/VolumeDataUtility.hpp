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
            std::cerr << "open failed." << std::endl;
            return false;
        }

        fin.seekg ( headerSize );

        if ( !data.read ( fin ) ) {
            std::cerr << "Error occured." << std::endl;
            return false;
        }

        return true;
    }
    template< typename T>
    static bool save ( VolumeData<T>& data, const std::string& filename )
    {
        std::ofstream fout ( filename.c_str(), std::ios::binary );

        if ( !fout ) {
            std::cerr << "open failed." << std::endl;
            return false;
        }

        if ( !data.write ( fout ) ) {
            std::cerr << "Error occured." << std::endl;
            return false;
        }

        return true;
    }

    template< typename T>
    static bool debug_save ( VolumeData<T>& data, const std::string& filename )
    {
        if ( !VolumeDataUtility::isDebugMode() ) {
            return true;        // do nothing.
        }

        VolumeDataUtility::save ( data, filename );
        std::cerr << "[debug] the result was saved to " << filename << std::endl;
        return true;
    }

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
            if ( data.get ( p ) == 0 ) {
                continue;
            }

            for ( const auto& d : nbr ) {
                if ( result.get ( p ) != 0 ) {
                    break;
                }

                if ( d == mi4::Point3i ( 0, 0, 0 ) ) {
                    continue;
                }

                const auto np = p + d;

                if ( !data.getInfo().isValid ( np ) ) {
                    continue;
                }

                if ( data.get ( np ) == 0 ) {
                    result.set ( p, 1 );
                }
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


    static  bool diff ( const VolumeData<char>& srcData, const VolumeData<char>& trgData, VolumeData<char>& outData )
    {
        const auto& info =  srcData.getInfo();

        if ( !outData.isReadable() ) {
            outData.init ( info );
        }

        for ( const auto& p : mi4::Range(info) ) {
            char value = 0;

            if ( srcData.get ( p ) == 1  && trgData.get ( p ) == 0 ) {
                value = 1;
            }

            outData.set ( p, value );
        }

        return true;
    }

    template< typename T>
    static bool negate_binary ( const VolumeData<T>& inData, VolumeData<T>& outData )
    {
        // data must be binary (0 or 1)
        auto& info = inData.getInfo();

        if ( !outData.isReadable() ) {
            outData.init ( info );
        }

        for ( const auto& p : mi4::Range(info) ) {
            outData.set ( p, 1 - inData.get ( p ) ) ;
        }

        return true;
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
        //mi::parallel_for_each ( range.begin(), range.end(), mi::erode ( inData, outData, r)   );
        std::for_each ( range.begin(), range.end(), morphology ( inData, result, r, false ) );
        return std::move ( result );
    }

    template<typename T>
    static VolumeData<T> dilate ( const VolumeData<T>& inData, const double r )
    {
        const auto& info = inData.getInfo();
        VolumeData<T> result ( info );
        Range range ( info );
        //mi::parallel_for_each ( range.begin(), range.end(), mi::erode ( inData, outData, r)   );
        std::for_each ( range.begin(), range.end(), morphology ( inData, result, r, true ) );
        return std::move ( result );
    }





};
}
#endif //MI4_VOLUME_DATA_UTILITY_HPP
