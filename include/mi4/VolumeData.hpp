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

namespace mi4 {
        using Vector3d = Eigen::Vector3d;
        using Vector3i = Eigen::Vector3i;
        using Vector3s = Eigen::Matrix< int16_t, 3, 1 >;
        using Point3d = Eigen::Vector3d;
        using Point3i = Eigen::Vector3i;

        class VolumeInfo {
        private:
                Point3i size_; ///< Global bounding box.
                Point3d pitch_; ///< Voxel pitch.
                Point3d origin_; ///< Origin point. Corresponding to global (0,0,0) in voxel space.
        public:
                explicit VolumeInfo (const Point3i& size = Point3i::Zero(), const Point3d& pitch = Point3d(1, 1, 1), const Point3d& origin = Point3d::Zero()) : size_(size), pitch_(pitch), origin_(origin)
                {
                }

                ~VolumeInfo (void) = default;
                VolumeInfo (const VolumeInfo& that) = default;
                VolumeInfo (VolumeInfo&& info) = default;
                VolumeInfo& operator = (const VolumeInfo& that) = default;
                VolumeInfo& operator = (VolumeInfo&& that) = default;

                VolumeInfo& setSize (const Point3i& size)
                {
                        this->size_ = size;
                        return *this;
                }

                VolumeInfo& setPitch (const Point3d& pitch)
                {
                        this->pitch_ = pitch;
                        return *this;
                }

                VolumeInfo& setOrigin (const Point3d& origin)
                {
                        this->origin_ = origin;
                        return *this;
                }

                VolumeInfo& init (const Point3i& size, const Point3d& pitch, const Point3d& origin)
                {
                        return this->setSize(size).setPitch(pitch).setOrigin(origin);
                }

                VolumeInfo& initByBoundingBox (const Vector3d& bmin, const Vector3d& bmax, const Point3d& pitch, const double offset)
                {
                        const auto off = Eigen::Vector3d::Constant(offset);
                        return this->setOrigin(bmin - off).setPitch(pitch).setSize(this->getPointInVoxelCeil(bmax - bmin + 2 * off));
                }

                Point3i getMin (void) const
                {
                        return Point3i(0, 0, 0);
                }
                Point3i getMax (void) const
                {
                        return this->getSize() - Point3i::Constant(1);
                }
                Point3i getSize (void) const
                {
                        return this->size_;
                }

                Point3d getPitch (void) const
                {
                        return this->pitch_;
                }

                Point3d getOrigin (void) const
                {
                        return this->origin_;
                }

                Point3d getPointInSpace (const Point3i& p) const
                {
                        return this->getPitch().cwiseProduct(p.cast< double >()) + this->getOrigin();
                }

                Point3i getPointInVoxel (const Point3d& p) const
                {
                        return this->getPointInVoxelFloor(p);
                }

                Point3i convertVectorCeil (const Vector3d& p) const
                {
                        return p.cwiseProduct(this->getPitch().cwiseInverse()).unaryExpr([] (double x) { return std::ceil(x); }).cast< int >();
                }


                Point3i convertVectorFloor (const Vector3d& p) const
                {
                        return p.cwiseProduct(this->getPitch().cwiseInverse()).unaryExpr([] (double x) { return std::floor(x); }).cast< int >();
                }

                Point3i getPointInVoxelCeil (const Point3d& p) const
                {
                        return this->convertVectorCeil(p - this->getOrigin());
                }

                Point3i getPointInVoxelFloor (const Point3d& p) const
                {
                        return this->convertVectorFloor(p - this->getOrigin());
                }

                Vector3d getVectorInSpace (const Vector3s& p) const
                {
                        return p.cast< double >().cwiseProduct(this->getPitch()) + this->getOrigin();
                }

                Vector3d getVector (const Vector3i& p) const
                {
                        return this->getPitch().cwiseProduct(p.cast< double >());
                }

                bool isValid (const Point3i& p) const
                {
                        return this->clamp(p) == p;
                }

                Point3i clamp (const Point3i& p) const
                {
                        return p.cwiseMax(this->getMin()).cwiseMin(this->getMax());
                }

                int64_t toIndex (const Point3i& p) const
                {
                        const auto& size = this->getSize();
                        return this->isValid(p) ? static_cast<int64_t> ( p.x()) + static_cast<int64_t> ( size.x()) * static_cast<int64_t> ( p.y() + p.z() * size.y()) : int64_t(-1);
                }

                Point3i fromIndex (const int64_t idx) const
                {
                        const auto& s = this->getSize();
                        return Point3i(static_cast<int> ( idx % s.x()), static_cast<int> ((idx % (s.x() * s.y())) / s.x()), static_cast<int> ( idx / (s.x() * s.y())));
                }

                float getLength (const Point3i& v) const
                {
                        return std::sqrt(this->getLengthSquared(v));
                }

                float getLength (const Vector3s& v) const
                {
                        return std::sqrt(this->getLengthSquared(v.cast< int >()));
                }

                float getLengthSquared (const Point3i& v) const
                {
                        return static_cast<float> ( this->getVector(v).squaredNorm());
                }

                bool isCorner (const Point3i& p) const
                {
                        return this->isValid(p) && (this->getMin().cwiseEqual(p).count() > 0 || this->getMax().cwiseEqual(p).count() > 0);
                }

                VolumeInfo clip (const Point3d& bmin, const Point3d& bmax) const
                {
                        const auto size = this->getPointInVoxelCeil(bmax) - this->getPointInVoxelFloor(bmin) + Point3i(1, 1, 1);
                        return VolumeInfo(size, this->getPitch(), bmin);
                }

                std::string toStringMetaData (void) const
                {
                        auto fn = [] (const auto& p) { return std::to_string(p.x()) + "x" + std::to_string(p.y()) + "x" + std::to_string(p.z()); };
                        return fn(this->getSize()) + "-" + fn(this->getPitch());
                }

                std::string toStringInfo (void) const
                {
                        std::stringstream ss;
                        ss << "size : " << this->getSize().transpose() << ", pitch : " << this->getPitch().transpose() << ", origin : " << this->getOrigin().transpose();
                        return ss.str();
                }
        };

        class Range {
        public:
                class iterator {
                public:
                        explicit iterator (const Range& range, const bool isBegin = true) : range_(range), pos_(range.getMin())
                        {
                                this->pos_.z() = isBegin ? this->pos_.z() : range.getMax().z() + 1;
                        }
                        iterator (const iterator& that) = default;
                        iterator (iterator&& that) = default;
                        iterator& operator = (const iterator& that) = default;
                        iterator& operator = (iterator&& that) = default;
                        virtual ~iterator (void) = default;

                        iterator& operator ++ (void)
                        {
                                this->step_forward();
                                return *this;
                        }

                        bool operator != (const iterator& rhs) const
                        {
                                return this->pos_ != rhs.pos_;
                        }

                        Point3i operator * (void) const
                        {
                                return this->pos_;
                        }

                        iterator& operator += (const size_t n)
                        {
                                for ( size_t i = 0; i < n; ++i ) {
                                        ++(*this);
                                }

                                return *this;
                        }
                private:
                        void step_forward (void)
                        {
                                const auto& bmin = this->range_.getMin();
                                const auto& bmax = this->range_.getMax();
                                auto& pos = this->pos_;

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
                        const Range& range_; // Pointer to the range.
                        Point3i pos_; // Current position.
                };

        public:
                explicit Range (const Point3i& bmin = Point3i(0, 0, 0), const Point3i& bmax = Point3i(0, 0, 0)) : bbox_(Eigen::AlignedBox3i(bmin, bmax))
                {

                }
                explicit Range (const VolumeInfo& info) : bbox_(Eigen::AlignedBox3i(info.getMin(), info.getMax()))
                {

                }
                Range (const Range& range) = default;
                Range& operator = (const Range& range) = default;
                Range (Range&& range) = default;
                Range& operator = (Range&& range) = default;
                ~Range (void) = default;

                Point3i getMin (void) const
                {
                        return this->bbox_.min();
                }

                Point3i getMax (void) const
                {
                        return this->bbox_.max();
                }

                Range::iterator begin (void)
                {
                        return Range::iterator(*this, true);
                }

                Range::iterator end (void)
                {
                        return Range::iterator(*this, false);
                }

        private:
                Eigen::AlignedBox3i bbox_;
        };

        template < typename T >
        class VolumeData {
        public:
                explicit VolumeData (const Point3i& size = Point3i(0, 0, 0), const bool allocateMemory = true)
                {
                        this->init(VolumeInfo(size), allocateMemory);
                }
                explicit VolumeData (const VolumeInfo& info, const bool allocateMemory = true)
                {
                        this->init(info, allocateMemory);
                }
                virtual ~VolumeData (void) = default;

                VolumeData (VolumeData&& that) = default;
                VolumeData& operator = (VolumeData&& that) = default;
                VolumeData (const VolumeData& that) = default;
                VolumeData& operator = (const VolumeData& that) = default;

                VolumeData< T >& init (const VolumeInfo& info, const bool allocateMemory = true)
                {
                        this->data_.clear();
                        this->info_ = info;

                        if ( allocateMemory ) {
                                this->allocate();
                        }

                        return *this;
                }

                VolumeData< T >& fill (const T& value = T())
                {
                        for ( const auto& p : Range(this->getInfo())) {
                                this->at(p) = value;
                        }

                        return *this;
                }

                const VolumeInfo& getInfo (void) const
                {
                        return this->info_;
                }

                Point3i getSize (void) const
                {
                        return this->getInfo().getSize();
                }

                T get (const Point3i& p) const
                {
                        return this->at(p);
                }

                void set (const Point3i& p, const T v)
                {
                        this->at(p) = v;
                }

                T at (const Point3i& p) const
                {
                        return this->at(p.x(), p.y(), p.z());
                }

                T& at (const Point3i& p)
                {
                        return this->at(p.x(), p.y(), p.z());
                }

                T at (const int x, const int y, const int z) const
                {
                        return this->data_.at(z).at(y).at(x);
                }

                T& at (const int x, const int y, const int z)
                {
                        return this->data_.at(z).at(y).at(x);
                }

                bool clone (const VolumeData< T >& that)
                {
                        this->init(that.getInfo(), true);

                        for ( const auto& p : Range(that.getInfo())) {
                                this->at(p) = that.at(p);
                        }

                        return true;
                }

                bool allocate (void)
                {
                        if ( !this->isReadable()) {
                                const auto& size = this->getInfo().getSize();
                                this->data_.assign(size.z(), std::vector< std::vector< T > >(size.y(), std::vector< T >(size.x(), T())));
                                if ( !this->isReadable()) {
                                        this->release();
                                        std::cerr << " error : allocation failed" << std::endl;
                                        return false;
                                }
                        }

                        return true;
                }

                void release (void)
                {
                        this->data_.erase(this->data_.begin(), this->data_.end());
                }

                bool isReadable (void) const
                {
                        const auto& size = this->info_.getSize();
                        if ( this->data_.size() != static_cast<size_t> ( size.z())) return false;
                        for ( const auto& dataxy : this->data_ ) {
                                if ( dataxy.size() != static_cast<size_t> ( size.y())) return false;
                                for ( const auto& datax : dataxy ) {
                                        if ( datax.size() != static_cast<size_t> ( size.x())) return false;
                                }
                        }
                        return true;
                }

                bool open (const std::string& filename, const size_t offset = 0)
                {
                        std::ifstream fin(filename.c_str(), std::ios::binary);
                        return this->read(fin, offset);
                }

                bool save (const std::string& filename)
                {
                        std::ofstream fout(filename.c_str(), std::ios::binary);
                        return this->write(fout);
                }

                bool read (std::ifstream& fin, const size_t offset = 0)
                {
                        if ( !fin ) {
                                std::cerr << " error : file stream is not ready yet." << std::endl;
                                return false;
                        }

                        fin.seekg(offset);

                        for ( auto& dataz : this->data_ ) {
                                for ( auto& datazy : dataz ) {
                                        if ( !fin.read(reinterpret_cast<char *> (datazy.data()), datazy.size() * sizeof(T))) {
                                                return false;
                                        }
                                }
                        }

                        return fin.good();
                }

                bool write (std::ofstream& fout)
                {
                        if ( !this->isReadable()) {
                                std::cerr << "volume data is not readable." << std::endl;
                                return false;
                        } else if ( !fout ) {
                                std::cerr << "the file cannot be open." << std::endl;
                                return false;
                        }

                        for ( const auto& dataz : this->data_ ) {
                                for ( const auto& datazy : dataz ) {
                                        if ( !fout.write(reinterpret_cast<const char *>(datazy.data()), sizeof(T) * datazy.size())) {
                                                return false;
                                        }
                                }
                        }
                        return fout.good();
                }
        private:
                VolumeInfo info_;
                std::vector< std::vector< std::vector< T > > > data_;
        };
}
#endif// MI_VOLUME_DATA_HPP