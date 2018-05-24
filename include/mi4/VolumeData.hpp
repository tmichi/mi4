/**
 * @file  VolumeData.hpp
 * @author Takashi Michikawa <michiawa@acm.org>
 */
#ifndef MI4_VOLUME_DATA_HPP
#define MI4_VOLUME_DATA_HPP 1

#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <vector>
#include <sstream>
#include <iomanip>

#include <Eigen/Dense>

namespace mi4
{
// Type definitions for vectors.
        typedef Eigen::Vector3d Vector3d;
        typedef Eigen::Vector3f Vector3f;
        typedef Eigen::Vector3i Vector3i;
        typedef Eigen::Matrix< short, 3, 1> Vector3s;
        typedef Eigen::Vector3d Point3d;
        typedef Eigen::Vector3f Point3f;
        typedef Eigen::Vector3i Point3i;
        typedef Eigen::Vector3f Color3f;

        class VolumeInfo
        {
        public:
                explicit VolumeInfo ( const Point3i& size = Point3i ( 0, 0, 0 ), const Point3d& pitch = Point3d ( 1, 1, 1 ), const Point3d& origin = Point3d ( 0, 0, 0 ) ) : _size ( size ), _pitch ( pitch ), _origin ( origin )
                {
                        return;
                }

                ~VolumeInfo ( void ) = default;
                VolumeInfo ( const VolumeInfo& that ) = default;
                VolumeInfo ( VolumeInfo&& info ) = default;
                VolumeInfo& operator = ( const VolumeInfo& that ) = default;
                VolumeInfo& operator = ( VolumeInfo&& that ) = default;

                VolumeInfo& setSize ( const Point3i& size )
                {
                        this->_size = size;
                        return *this;
                }

                VolumeInfo& setPitch ( const Point3d& pitch )
                {
                        this->_pitch = pitch;
                        return *this;
                }

                VolumeInfo& setOrigin ( const Point3d& origin )
                {
                        this->_origin = origin;
                        return *this;
                }

                VolumeInfo& init ( const Point3i& size, const Point3d& pitch, const Point3d& origin )
                {
                        return this->setSize ( size ).setPitch ( pitch ).setOrigin ( origin );
                }

                VolumeInfo&
                initByBoundingBox ( const Vector3d& bmin, const Vector3d& bmax, const Point3d& pitch, const double offset )
                {
                        const auto origin = bmin - Vector3d ( offset, offset, offset );
                        Point3i size;
                        size.x() = this->ceil_int ( ( bmax.x() - bmin.x() + 2 * offset ) / pitch.x() ) ;
                        size.y() = this->ceil_int ( ( bmax.y() - bmin.y() + 2 * offset ) / pitch.y() ) ;
                        size.z() = this->ceil_int ( ( bmax.z() - bmin.z() + 2 * offset ) / pitch.z() ) ;
                        return this->init ( size, pitch, origin );
                }

                Point3i getMin ( void ) const
                {
                        return Point3i ( 0, 0, 0 );
                }
                Point3i getMax ( void ) const
                {
                        return this->getSize() - Point3i ( 1, 1, 1 );
                }
                Point3i getSize ( void ) const
                {
                        return this->_size;
                }
                Point3d getPitch ( void ) const
                {
                        return this->_pitch;
                }
                Point3d getOrigin ( void ) const
                {
                        return this->_origin;
                }


                Point3d getPointInSpace ( const Point3i& p ) const
                {
                        const auto& pitch = this->getPitch();
                        return Point3d ( pitch.x() * p.x(), pitch.y() * p.y(),	pitch.z() * p.z() ) + this->getOrigin();
                }

                Point3i getPointInVoxel ( const Point3d& p ) const
                {
                        const auto& pitch = this->getPitch();
                        const auto v = p - this->getOrigin();
                        return  Point3i (
                                        static_cast<int> ( v.x() / pitch.x() ),
                                        static_cast<int> ( v.y() / pitch.y() ),
                                        static_cast<int> ( v.z() / pitch.z() )
                                );
                }
                Point3i convertVectorCeil ( const Vector3d& p ) const
                {
                        const auto& pitch = this->getPitch();
                        return Point3i (
                                       this->ceil_int ( p.x() / pitch.x() ),
                                       this->ceil_int ( p.y() / pitch.y() ),
                                       this->ceil_int ( p.z() / pitch.z() )
                               );
                }
                Point3i convertVectorFloor ( const Vector3d& p ) const
                {
                        const auto& pitch = this->getPitch();
                        return Point3i (
                                       this->floor_int ( p.x() / pitch.x() ),
                                       this->floor_int ( p.y() / pitch.y() ),
                                       this->floor_int ( p.z() / pitch.z() )
                               );
                }
                Point3i getPointInVoxelCeil ( const Point3d& p ) const
                {
                        const auto& pitch = this->getPitch();
                        const auto v = p - this->getOrigin();
                        return Point3i (
                                       this->ceil_int ( v.x() / pitch.x() ),
                                       this->ceil_int ( v.y() / pitch.y() ),
                                       this->ceil_int ( v.z() / pitch.z() )
                               );
                }
                Point3i getPointInVoxelFloor ( const Point3d& p ) const
                {
                        const auto& pitch = this->getPitch();
                        const auto v = p - this->getOrigin();
                        return Point3i (
                                       this->floor_int ( v.x() / pitch.x() ),
                                       this->floor_int ( v.y() / pitch.y() ),
                                       this->floor_int ( v.z() / pitch.z() )
                               );
                }

                Vector3d getVectorInSpace ( const Vector3s& p ) const
                {
                        return this->getVector ( this->to_vector3i ( p ) ) + this->getOrigin();
                }

                Vector3d getVector ( const Vector3i& p ) const
                {
                        const auto& pitch = this->getPitch();
                        return Vector3d ( pitch.x() * p.x(), pitch.y() * p.y(),	pitch.z() * p.z() );
                }

                bool isValid ( const Point3i& p ) const
                {
                        const auto& bmin = this->getMin();
                        const auto& bmax = this->getMax();
                        return ( bmin.x() <= p.x() && p.x() <= bmax.x() &&
                                 bmin.y() <= p.y() && p.y() <= bmax.y() &&
                                 bmin.z() <= p.z() && p.z() <= bmax.z() );
                }

                Point3i
                clamp ( const Point3i& p ) const
                {
                        const auto& bmin = this->getMin ();
                        const auto& bmax = this->getMax ();
                        const int x = this->clamp_int ( p.x(), bmin.x(), bmax.x() );
                        const int y = this->clamp_int ( p.y(), bmin.y(), bmax.y() );
                        const int z = this->clamp_int ( p.z(), bmin.z(), bmax.z() );
                        return Point3i ( x, y, z );
                }

                int toIndex ( const  Point3i& p ) const
                {
                        if (  this->isValid ( p ) ) {
                                const auto& size = this->getSize();
                                return p.x() + size.x() * ( p.y() + p.z() * size.y() ) ;
                        } else {
                                return -1; // invalid point id
                        }
                }
                Point3i fromIndex ( const int idx ) const
                {
                        const auto& size = this->getSize ();
                        return Point3i (
                                       static_cast<int> ( idx % size.x() ),
                                       static_cast<int> ( ( idx % ( size.x() * size.y() ) ) / size.x() ),
                                       static_cast<int> ( idx / ( size.x() * size.y() ) )
                               );
                }

                float getLength ( const Point3i& v ) const
                {
                        return std::sqrt ( this->getLengthSquared ( v ) );
                }

                float getLength ( const Vector3s& v ) const
                {
                        return  this->getLength ( this->to_vector3i ( v ) );
                }

                float getLengthSquared ( const Point3i& v ) const
                {
                        return static_cast<float> ( this->getVector ( v ).squaredNorm() );
                }

                bool
                isCorner ( const Point3i& p ) const
                {
                        if ( ! this->isValid ( p ) ) {
                                return false;
                        }

                        const auto& bmin = this->getMin();
                        const auto& bmax = this->getMax();

                        return p.x() == bmin.x() || p.y() == bmin.y() || p.z() == bmin.z() ||
                               p.x() == bmax.x() || p.y() == bmax.y() || p.z() == bmax.z() ;
                }


                void
                clip ( const Point3d& bmin, const Point3d& bmax, VolumeInfo& info )
                {
                        const auto& pitch = this->getPitch();
                        const auto pmin = this->getPointInVoxelFloor ( bmin - this->getOrigin() );
                        const auto pmax = this->getPointInVoxelCeil ( bmax - this->getOrigin() );
                        const auto size = pmax - pmin + Point3i ( 1, 1, 1 );
                        info.init ( size, pitch, bmin );
                }

                std::string toStringInfo ( void ) const
                {
                        std::stringstream ss;
                        const auto& size = this->getSize();
                        const auto& p = this->getPitch();
                        ss << size.x() << "x" << size.y() << "x" << size.z() << "-"
                           << std::setprecision ( 4 ) << p.x() << "x"
                           << std::setprecision ( 4 ) << p.y() << "x"
                           << std::setprecision ( 4 ) << p.z();
                        return ss.str();
                }

        private:
                inline Vector3i to_vector3i ( const Vector3s& v ) const
                {
                        return Vector3i ( v.x(), v.y(), v.z() );
                }

                inline int ceil_int ( const double v ) const
                {
                        return static_cast<int> ( std::ceil ( v ) );
                }

                inline int floor_int ( const double v ) const
                {
                        return static_cast<int> ( std::floor ( v ) );
                }

                inline int clamp_int ( const int input, const int lower, const int upper ) const
                {
                        if ( input <  lower ) {
                                return lower;
                        } else if ( upper < input ) {
                                return upper;
                        } else {
                                return input;
                        }
                }
        private:
                Point3i _size; ///< Global bounding box.
                Point3d _pitch; ///< Voxel pitch.
                Point3d _origin; ///< Origin point. Corresponding to global (0,0,0) in voxel space.
        };

        class Range
        {
        public:
                class iterator
                {
                public:
                        iterator ( Range* range, const bool isBegin = true ) : _range ( range ), _pos ( range->getMin() )
                        {
                                if ( !isBegin ) {
                                        this->_pos.z() = range->getMax().z() + 1;
                                }
                        }
                        iterator ( const iterator& that ) = default;
                        iterator ( iterator&& that ) = default;
                        iterator& operator = ( const iterator& that ) = default;
                        iterator& operator = ( iterator&& that ) = default;
                        virtual ~iterator ( void ) = default;

                        iterator& operator ++ ( void )
                        {
                                this->step_forward();
                                return *this;
                        }

                        bool operator != ( const iterator& rhs ) const
                        {
                                return this->_pos != rhs._pos;
                        }

                        Point3i operator * ( void ) const
                        {
                                return this->_pos;
                        }

                        iterator operator + ( const int n )
                        {
                                iterator tmp ( *this );
                                tmp.operator += ( n );
                                return tmp; // @todo check
                        }

                        iterator& operator += ( const int n )
                        {
                                iterator& iter = *this;

                                for ( int i = 0 ; i < n ; ++i ) {
                                        ++iter;
                                }

                                return *this;
                        }
                private:
                        inline void step_forward ( void )
                        {
                                const auto& bmin = this->_range->getMin();
                                const auto& bmax = this->_range->getMax();
                                auto& pos = this->_pos;

                                if ( bmax.z() < pos.z() ) {
                                        return;        // do nothing
                                }

                                pos.x() += 1;

                                if ( bmax.x() < pos.x() ) {
                                        pos.x() = bmin.x();
                                        pos.y() += 1;

                                        if ( bmax.y() < pos.y() ) {
                                                pos.y() = bmin.y();
                                                pos.z() += 1;
                                        }
                                }

                                return;
                        }
                private:
                        Range* _range; // Pointer to the range.
                        Point3i _pos; // Current position.
                };

        public:
                explicit Range ( const Point3i& bmin = Point3i ( 0, 0, 0 ), const Point3i& bmax = Point3i ( 0, 0, 0 ) ) : _bmin ( bmin ), _bmax ( bmax )
                {
                        return;
                }
                explicit Range ( const VolumeInfo& info ) : _bmin ( info.getMin() ), _bmax ( info.getMax() )
                {
                        return;
                }
                Range ( const Range& range ) = default;
                Range& operator = ( const Range& range ) = default;
                Range ( Range&& range ) = default;
                Range& operator = ( Range&& range ) = default;
                ~Range ( void ) = default;

                bool check ( void ) const
                {
                        const auto& bmin = this->getMin();
                        const auto& bmax = this->getMax();
                        return  bmin.x() <= bmax.x() && bmin.y() <= bmax.y() && bmin.z() <= bmax.z();
                }
                virtual bool isValid ( const Point3i& p ) const
                {
                        const auto& bmin = this->getMin();
                        const auto& bmax = this->getMax();
                        return ( bmin.x() <= p.x() && p.x() <= bmax.x() &&
                                 bmin.y() <= p.y() && p.y() <= bmax.y() &&
                                 bmin.z() <= p.z() && p.z() <= bmax.z() );
                }

                Point3i getMin ( void ) const
                {
                        return this->_bmin;
                }

                Point3i getMax ( void ) const
                {
                        return this->_bmax;
                }


                bool getOverlap ( const Range& range, Range& result )
                {
                        const auto& bmin0 = this->getMin();
                        const auto& bmax0 = this->getMax();
                        const auto& bmin1 = range.getMin();
                        const auto& bmax1 = range.getMax();
                        auto& bmin = result._bmin;
                        auto& bmax = result._bmax;
                        return  this->get_overlap_1d ( bmin0.x(), bmax0.x(), bmin1.x(), bmax1.x(),  bmin.x(), bmax.x() ) &&
                                this->get_overlap_1d ( bmin0.y(), bmax0.y(), bmin1.y(), bmax1.y(),  bmin.y(), bmax.y() ) &&
                                this->get_overlap_1d ( bmin0.z(), bmax0.z(), bmin1.z(), bmax1.z(),  bmin.z(), bmax.z() );
                }

                Range::iterator begin ( void )
                {
                        return Range::iterator ( this, true );
                }

                Range::iterator end ( void )
                {
                        return Range::iterator ( this, false );
                }
        private:
                inline bool get_overlap_1d ( const int mn0, const int mx0, const int mn1, const int mx1, int& mn, int& mx )
                {
                        if ( mx1 <= mn0 ) {
                                mn = mx1;
                                mx = mn0;
                                return true;
                        } else if ( mx0 <= mn1 ) {
                                mn = mx0;
                                mx = mn1;
                                return true;
                        } else {
                                return false;
                        }
                }
        private:
                Point3i _bmin;
                Point3i _bmax;
        };

//
        template <typename T>
        class VolumeData
        {
        public:
                explicit VolumeData ( void )
                {
                        this->init ( VolumeInfo ( Point3i ( 0, 0, 0 ) ), false );
                        return;
                }
                explicit VolumeData ( const Point3i& size, const bool allocateMemory = true )
                {
                        this->init ( VolumeInfo ( size ), allocateMemory );
                        return;
                }
                explicit VolumeData ( const VolumeInfo& info, const bool allocateMemory = true )
                {
                        this->init ( info, allocateMemory );
                        return;
                }
                virtual ~VolumeData ( void ) = default;

                VolumeData ( VolumeData&& that ) = default;
                VolumeData& operator = ( VolumeData&& that ) = default;
                VolumeData ( const VolumeData& that ) = default;
                VolumeData& operator = ( const VolumeData& that ) = default;

                VolumeData<T>& init ( const VolumeInfo& info, const bool allocateMemory = true )
                {
                        this->release();
                        this->getInfo().init ( info.getSize(), info.getPitch(), info.getOrigin() );

                        if ( allocateMemory ) {
                                this->allocate();
                        }

                        return *this;
                }

                VolumeData<T>& fill ( const T& value )
                {
                        for ( const auto& p : Range ( this->getInfo() ) ) {
                                this->set ( p, value );
                        }

                        return *this;
                }

                inline VolumeInfo& getInfo ( void )
                {
                        return this->_info;
                }

                inline VolumeInfo getInfo ( void ) const
                {
                        return this->_info;
                }

                inline Point3i getSize ( void ) const
                {
                        return this->getInfo().getSize();
                }

                inline T get ( const Point3i& p ) const
                {
                        return this->at ( p.x(), p.y(), p.z()  );
                }

                inline T get ( const int x, const int y, const int z ) const
                {
                        return this->at ( x, y, z );
                }

                inline void set ( const Point3i& p, const T v )
                {
                        this->at ( p.x(), p.y(), p.z() ) = v ;
                }

                inline void set ( const int x, const int y, const int z, const T v )
                {
                        this->at ( x, y, z ) = v;
                }

                inline T at ( const Point3i& p ) const
                {
                        return this->at ( p.x(), p.y(), p.z() );
                }

                inline T& at ( const Point3i& p )
                {
                        return this->at ( p.x(), p.y(), p.z() );
                }

                inline T at ( const int x, const int y, const int z ) const
                {
                        return this->_data.at ( z ).at ( y ).at ( x );
                }

                inline T& at ( const int x, const int y, const int z )
                {
                        return this->_data.at ( z ).at ( y ).at ( x );
                }

                void clear ( void )
                {
                        this->fill ( T() );
                        return;
                }

                bool clone ( const VolumeData<T>& that )
                {
                        const auto& info = that.getInfo();
                        this->init ( info, true );

                        for ( const auto&& p : Range ( info ) ) {
                                this->set ( p, that.get ( p ) );
                        }

                        return true;
                }

                bool allocate ( void )
                {
                        const auto& size = this->_info.getSize();

                        if ( ! this->isReadable() ) {
                                this->_isReadable = false;
                                this->_data.assign ( size.z(), std::vector<std::vector<T> > ( size.y(), std::vector<T> ( size.x(), T() ) ) );

                                if ( this->_data.size() != static_cast<size_t> ( size.z() ) ) {
                                        std::cerr << " volume data allocation failed" << std::endl;
                                        return false;        // when allocation is failed.
                                }

                                this->_isReadable = true;
                        }

                        return true;
                }

                bool release ( void )
                {
                        this->_isReadable = false;
                        this->_data.erase ( this->_data.begin(), this->_data.end() );
                        return true;
                }

                bool isReadable ( void ) const
                {
                        return this->_isReadable;
                }

                bool open ( const std::string& filename )
                {
                        std::ifstream fin ( filename.c_str() );

                        if ( !fin ) {
                                return false;
                        }

                        return this->read ( fin );
                }

                bool save ( const std::string& filename )
                {
                        std::ofstream fout ( filename.c_str() );

                        if ( !fout ) {
                                return false;
                        }

                        return this->write ( fout );
                }


                bool read ( std::ifstream& fin )
                {
                        if ( !this->isReadable() ) {
                                std::cerr << "error : volume data cannot be read. memory space is not allocated yet." << std::endl;
                                return false;
                        }

                        if ( !fin ) {
                                std::cerr << " error : file stream is not ready yet." << std::endl;
                                return false;
                        }

                        const Point3i& size = this->getInfo().getSize();

                        for ( int z = 0 ; z < size.z() ; ++z ) {
                                std::vector<T> buffer ( size.x() * size.y(), T() );

                                if ( !fin.read ( ( char* ) & ( buffer[0] ), buffer.size() * sizeof ( T ) ) ) {
                                        std::cerr << "reading data failed." << std::endl;
                                        return false;
                                }

                                for ( int y = 0 ; y < size.y() ; ++y ) {
                                        for ( int x = 0 ; x < size.x() ; ++x ) {
                                                this->at ( x, y, z ) = buffer[ x +  y * size.x() ];
                                        }
                                }
                        }

                        return fin.good();
                }

                bool write ( std::ofstream& fout )
                {
                        const auto& size = this->getInfo().getSize();

                        if ( ! this->isReadable() ) {
                                std::cerr << "volume data is not readable." << std::endl;
                                return false;
                        }

                        if ( !fout ) {
                                std::cerr << "the file cannot be open." << std::endl;
                                return false;
                        }

                        for ( int z = 0 ; z < size.z() ; ++z ) {
                                const auto bufSize = sizeof ( T ) * size.x() * size.y();
                                std::vector<T> buf;
                                buf.reserve ( size.x()*size.y() );

                                for ( int y = 0 ; y < size.y() ; ++y ) {
                                        for ( int x = 0 ; x < size.x() ; ++x )  {
                                                buf.push_back ( this->at ( x, y, z ) );
                                        }
                                }

                                if ( !fout.write ( ( char* ) ( &buf[0] ), bufSize ) ) {
                                        std::cerr << "writing data failed." << std::endl;
                                        return false;
                                }
                        }

                        return fout.good();
                }

                VolumeData<T> clip ( const Point3i& bmin, const Point3i& bmax ) const
                {
                        VolumeData<T> result ( VolumeInfo ( bmax - bmin + Point3i ( 1, 1, 1 ) ) );

                        for ( const auto& p : Range ( result.getInfo() ) ) {
                                if ( !this->getInfo().isValid ( bmin + p ) ) {
                                        continue;
                                }

                                result.set ( p, this->get ( bmin + p ) );
                        }

                        return std::move ( result );
                }
        private:
                VolumeInfo _info;
                std::vector<std::vector<std::vector<T> > > _data;
                bool _isReadable;
        };
}
#endif// MI_VOLUME_DATA_HPP
