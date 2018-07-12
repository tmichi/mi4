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
#include <Eigen/Geometry>

namespace mi4
{
        using Vector3d = Eigen::Vector3d;
        using Vector3i = Eigen::Vector3i;
        using Vector3s = Eigen::Matrix< int16_t, 3, 1 >;
        using Point3d = Eigen::Vector3d;
        using Point3i = Eigen::Vector3i;

        static auto ceili = [] (const double v) {
                return static_cast<int> ( std::ceil(v));
        };
        static auto floori = [] (const double v) {
                return static_cast<int> ( std::floor(v));
        };
        static auto clampi = [] (const int v, const int l, const int u) {
                return std::min(std::max(v, l), u);
        };

        class VolumeInfo
        {
        private:
                Point3i _size; ///< Global bounding box.
                Point3d _pitch; ///< Voxel pitch.
                Point3d _origin; ///< Origin point. Corresponding to global (0,0,0) in voxel space.
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

                VolumeInfo& initByBoundingBox (const Vector3d& bmin, const Vector3d& bmax, const Point3d& pitch, const double offset)
                {
                        const Vector3d offv(offset, offset, offset);
                        return this->setOrigin(bmin - offv).setPitch(pitch).setSize(this->getPointInVoxelCeil(bmax - bmin + 2 * offv));
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
                        return Point3d(pitch.x() * p.x(), pitch.y() * p.y(), pitch.z() * p.z()) + this->getOrigin();
                }

                Point3i getPointInVoxel ( const Point3d& p ) const
                {
                        return this->getPointInVoxelFloor(p);
                }

                Point3i convertVectorCeil ( const Vector3d& p ) const
                {
                        const auto& pitch = this->getPitch();
                        return Point3i(ceili(p.x() / pitch.x()), ceili(p.y() / pitch.y()), ceili(p.z() / pitch.z()));
                }

                Point3i convertVectorFloor ( const Vector3d& p ) const
                {
                        const auto& pitch = this->getPitch();
                        return Point3i(floori(p.x() / pitch.x()), floori(p.y() / pitch.y()), floori(p.z() / pitch.z()));
                }

                Point3i getPointInVoxelCeil ( const Point3d& p ) const
                {
                        const auto& pitch = this->getPitch();
                        const auto v = p - this->getOrigin();
                        return Point3i(ceili(v.x() / pitch.x()), ceili(v.y() / pitch.y()), ceili(v.z() / pitch.z()));
                }

                Point3i getPointInVoxelFloor ( const Point3d& p ) const
                {
                        const auto& pitch = this->getPitch();
                        const auto v = p - this->getOrigin();
                        return Point3i(floori(v.x() / pitch.x()), floori(v.y() / pitch.y()), floori(v.z() / pitch.z()));
                }

                Vector3d getVectorInSpace ( const Vector3s& p ) const
                {
                        return this->getVector ( this->to_vector3i ( p ) ) + this->getOrigin();
                }

                Vector3d getVector ( const Vector3i& p ) const
                {
                        const auto& pitch = this->getPitch();
                        return Vector3d(pitch.x() * p.x(), pitch.y() * p.y(), pitch.z() * p.z());
                }

                bool isValid ( const Point3i& p ) const
                {
                        return this->clamp(p) == p;
                }

                Point3i clamp (const Point3i& p) const
                {
                        const auto& bmin = this->getMin();
                        const auto& bmax = this->getMax();
                        return Point3i(clampi(p.x(), bmin.x(), bmax.x()), clampi(p.y(), bmin.y(), bmax.y()), clampi(p.z(), bmin.z(), bmax.z()));
                }

                int64_t toIndex (const Point3i& p) const
                {
                        if ( this->isValid(p)) {
                                const auto& size = this->getSize();
                                return static_cast<int64_t> ( p.x()) + static_cast<int64_t> ( size.x()) * static_cast<int64_t> ( p.y() + p.z() * size.y());
                        } else {
                                return -1; // invalid point id
                        }
                }

                Point3i fromIndex (const int64_t idx) const
                {
                        const auto& s = this->getSize();
                        return Point3i(static_cast<int> ( idx % s.x()), static_cast<int> ((idx % (s.x() * s.y())) / s.x()), static_cast<int> ( idx / (s.x() * s.y())));
                }

                float getLength ( const Point3i& v ) const
                {
                        return std::sqrt ( this->getLengthSquared ( v ) );
                }

                float getLength ( const Vector3s& v ) const
                {
                        return this->getLength(this->to_vector3i(v));
                }

                float getLengthSquared ( const Point3i& v ) const
                {
                        return static_cast<float> ( this->getVector ( v ).squaredNorm() );
                }

                bool isCorner (const Point3i& p) const
                {
                        const auto& bmin = this->getMin();
                        const auto& bmax = this->getMax();
                        return this->isValid(p) && (p.x() == bmin.x() || p.y() == bmin.y() || p.z() == bmin.z() || p.x() == bmax.x() || p.y() == bmax.y() || p.z() == bmax.z());
                }

                VolumeInfo clip (const Point3d& bmin, const Point3d& bmax) const
                {
                        return VolumeInfo(this->getPointInVoxelCeil(bmax) - this->getPointInVoxelFloor(bmin) + Point3i(1, 1, 1), this->getPitch(), bmin);
                }

                std::string toStringInfo ( void ) const
                {
                        std::stringstream ss;
                        ss << "size : " << this->getSize().transpose() << ", pitch : " << this->getPitch().transpose() << ", origin : " << this->getOrigin().transpose();
                        return ss.str();
                }

        private:
                inline Vector3i to_vector3i ( const Vector3s& v ) const
                {
                        return Vector3i ( v.x(), v.y(), v.z() );
                }
        };

        class Range
        {
        public:
                class iterator
                {
                public:
                        explicit iterator (const Range& range, const bool isBegin = true) : _range(range), _pos(range.getMin())
                        {
                                this->_pos.z() = isBegin ? this->_pos.z() : range.getMax().z() + 1;
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

                        iterator& operator += (const size_t n)
                        {
                                for ( int i = 0; i < n; ++i ) {
                                        ++(*this);
                                }

                                return *this;
                        }
                private:
                        void step_forward (void)
                        {
                                const auto& bmin = this->_range.getMin();
                                const auto& bmax = this->_range.getMax();
                                auto& pos = this->_pos;

                                if ( pos.z() <= bmax.z()) {
                                        pos.x() += 1;

                                        if ( bmax.x() < pos.x()) {
                                                pos.x() = bmin.x();
                                                pos.y() += 1;

                                                if ( bmax.y() < pos.y()) {
                                                        pos.y() = bmin.y();
                                                        pos.z() += 1;
                                                }
                                        }
                                }
                        }
                private:
                        const Range& _range; // Pointer to the range.
                        Point3i _pos; // Current position.
                };

        public:
                explicit Range (const Point3i& bmin = Point3i(0, 0, 0), const Point3i& bmax = Point3i(0, 0, 0)) : _bbox(Eigen::AlignedBox3i(bmin, bmax)) {}
                explicit Range (const VolumeInfo& info) : _bbox(Eigen::AlignedBox3i(info.getMin(), info.getMax())) {}
                Range ( const Range& range ) = default;
                Range& operator = ( const Range& range ) = default;
                Range ( Range&& range ) = default;
                Range& operator = ( Range&& range ) = default;
                ~Range ( void ) = default;

                virtual bool isValid ( const Point3i& p ) const
                {
                        return this->_bbox.contains(p);
                }

                Point3i getMin ( void ) const
                {
                        return this->_bbox.min();
                }

                Point3i getMax ( void ) const
                {
                        return this->_bbox.max();
                }

                bool getOverlap ( const Range& range, Range& result )
                {
                        result._bbox = this->_bbox.clamp(range._bbox);
                        return !result._bbox.isEmpty();
                }

                Range::iterator begin ( void )
                {
                        return Range::iterator(*this, true);
                }

                Range::iterator end ( void )
                {
                        return Range::iterator(*this, false);
                }
        private:
                Eigen::AlignedBox3i _bbox;
        };

        template < typename T >
        class VolumeData
        {
        public:
                explicit VolumeData (const Point3i& size = Point3i(0, 0, 0), const bool allocateMemory = true)
                {
                        this->init ( VolumeInfo ( size ), allocateMemory );
                }
                explicit VolumeData ( const VolumeInfo& info, const bool allocateMemory = true )
                {
                        this->init ( info, allocateMemory );
                }
                virtual ~VolumeData ( void ) = default;

                VolumeData ( VolumeData&& that ) = default;
                VolumeData& operator = ( VolumeData&& that ) = default;
                VolumeData ( const VolumeData& that ) = default;
                VolumeData& operator = ( const VolumeData& that ) = default;

                VolumeData< T >& init (const VolumeInfo& info, const bool allocateMemory = true)
                {
                        this->_data.clear();
                        this->_info = info;

                        if ( allocateMemory ) {
                                this->allocate();
                        }

                        return *this;
                }

                VolumeData< T >& fill (const T& value = T())
                {
                        for ( const auto& p : Range ( this->getInfo() ) ) {
                                this->set ( p, value );
                        }

                        return *this;
                }

                const VolumeInfo& getInfo (void) const
                {
                        return this->_info;
                }

                Point3i getSize (void) const
                {
                        return this->getInfo().getSize();
                }

                T get (const Point3i& p) const
                {
                        return this->at(p.x(), p.y(), p.z());
                }

                void set (const Point3i& p, const T v)
                {
                        this->at(p.x(), p.y(), p.z()) = v;
                }

                T at (const Point3i& p) const
                {
                        return this->at ( p.x(), p.y(), p.z() );
                }

                T& at (const Point3i& p)
                {
                        return this->at ( p.x(), p.y(), p.z() );
                }

                T at (const int x, const int y, const int z) const
                {
                        return this->_data.at ( z ).at ( y ).at ( x );
                }

                T& at (const int x, const int y, const int z)
                {
                        return this->_data.at ( z ).at ( y ).at ( x );
                }

                bool clone (const VolumeData< T >& that)
                {
                        this->init(that.getInfo(), true);

                        for ( const auto& p : Range(that.getInfo())) {
                                this->set ( p, that.get ( p ) );
                        }

                        return true;
                }

                bool allocate ( void )
                {
                        if ( !this->isReadable()) {
                                const auto& size = this->_info.getSize();

                                if ( size.x() * size.y() * size.z() == 0 ) {
                                        return false;
                                }

                                this->_data.assign(size.z(), std::vector< std::vector< T > >(size.y(), std::vector< T >(size.x(), T())));
                                bool allocated = (this->_data.size() == static_cast<size_t> ( size.z()));

                                for ( const auto& dataxy : this->_data ) {
                                        allocated &= dataxy.size() == static_cast<size_t> ( size.y());

                                        for ( const auto& datax : dataxy ) {
                                                allocated &= datax.size() == static_cast<size_t> ( size.x());
                                        }
                                }

                                if ( !allocated ) {
                                        this->release();
                                        std::cerr << " error : allocation failed" << std::endl;
                                        return false;
                                }
                        }

                        return true;
                }

                void release (void)
                {
                        this->_data.erase ( this->_data.begin(), this->_data.end() );
                }

                bool isReadable ( void ) const
                {
                        return this->_data.size() == static_cast<size_t> ( this->getInfo().getSize().z());
                }

                bool open (const std::string& filename, const size_t offset = 0)
                {
                        std::ifstream fin ( filename.c_str() );
                        return this->read(fin, offset);
                }

                bool save ( const std::string& filename )
                {
                        std::ofstream fout ( filename.c_str() );
                        return this->write ( fout );
                }

                bool read (std::ifstream& fin, const size_t offset = 0)
                {
                        if ( !fin ) {
                                std::cerr << " error : file stream is not ready yet." << std::endl;
                                return false;
                        }

                        fin.seekg(offset);
                        const auto& size = this->getInfo().getSize();

                        for ( int z = 0; z < size.z(); ++z ) {
                                for ( int y = 0; y < size.y(); ++y ) {
                                        if ( !fin.read((char *) (this->_data.at(z).at(y).data()), size.x() * sizeof(T))) {
                                                std::cerr << "reading data failed." << std::endl;
                                                return false;
                                        }
                                }
                        }

                        return fin.good();
                }

                bool write ( std::ofstream& fout )
                {
                        if ( !this->isReadable()) {
                                std::cerr << "volume data is not readable." << std::endl;
                                return false;
                        }

                        if ( !fout ) {
                                std::cerr << "the file cannot be open." << std::endl;
                                return false;
                        }

                        const auto& size = this->getInfo().getSize();

                        for ( int z = 0; z < size.z(); ++z ) {
                                for ( int y = 0; y < size.y(); ++y ) {
                                        if ( !fout.write((char *) (this->_data.at(z).at(y).data()), sizeof(T) * size.x())) {
                                                std::cerr << "writing data failed." << std::endl;
                                                return false;
                                        }
                                }
                        }

                        return fout.good();
                }
        private:
                VolumeInfo _info;
                std::vector< std::vector< std::vector< T > > > _data;
        };
}
#endif// MI_VOLUME_DATA_HPP