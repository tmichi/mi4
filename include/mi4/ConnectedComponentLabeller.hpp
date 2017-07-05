#ifndef MI_CONNECTED_COMPONENT_LABELLER_HPP
#define MI_CONNECTED_COMPONENT_LABELLER_HPP 1
#include <vector>
#include <cstdlib>
#include <mi4/VolumeData.hpp>
namespace mi4
{
        class ConnectedComponentLabeller
        {
        private:
                typedef int id_t;
                class RunLengthCodeBinary
                {
                private:
                        mi4::Vector3s _start;
                        short int _length;
                        id_t _parent; // -1 : root, others: id
                public:
                        explicit RunLengthCodeBinary ( const mi4::Vector3s& start, const short int length ): _start ( start ), _length ( length ), _parent ( -1 )
                        {
                                return;
                        }

                        RunLengthCodeBinary ( const RunLengthCodeBinary& that ) = default;
                        RunLengthCodeBinary ( RunLengthCodeBinary&& that ) = default;
                        RunLengthCodeBinary& operator = ( const RunLengthCodeBinary& that ) = default;
                        RunLengthCodeBinary& operator = ( RunLengthCodeBinary&& that ) = default;
                        ~RunLengthCodeBinary ( void ) = default;

                        void setParent ( const id_t parent )
                        {
                                this->_parent = parent;
                                return;
                        }
                        id_t getParent ( void ) const
                        {
                                return this->_parent;
                        }
                        bool isConnected ( const RunLengthCodeBinary& that )
                        {
                                return this->is_connected_26 ( that );
                        }
                        short int getLength ( void ) const
                        {
                                return this->_length;
                        }
                        const mi4::Vector3s& getStart ( void ) const
                        {
                                return this->_start;
                        }
                private:
                        bool is_connected_26 ( const RunLengthCodeBinary& that )
                        {
                                const auto& start0 = this->getStart();
                                const auto sx0 = start0.x();
                                const auto sy0 = start0.y();
                                const auto sz0 = start0.z();

                                const auto& start1 = that.getStart();
                                const auto sx1 = start1.x();
                                const auto sy1 = start1.y();
                                const auto sz1 = start1.z();

                                const auto length0 = this->getLength();
                                const auto length1 = that.getLength();

                                const auto dy = std::abs ( sy0 - sy1 );
                                const auto dz = std::abs ( sz0 - sz1 );

                                if ( dy > 1 || dz > 1 ) {
                                        return false;
                                }

                                const auto mn0 = sx0;
                                const auto mx0 = sx0 + length0 - 1;

                                const auto mn1 = sx1;
                                const auto mx1 = sx1 + length1 - 1;

                                return  ( mn0 - 1 <= mn1 && mn1 <= mx0 + 1 ) ||
                                        ( mn0 - 1 <= mx1 && mx1 <= mx0 + 1 ) ||
                                        ( mn1 - 1 <= mn0 && mn0 <= mx1 + 1 ) ||
                                        ( mn1 - 1 <= mx0 && mx0 <= mx1 + 1 ) ;
                        }
                };

                template <typename T>
                class RunLengthObject
                {
                private:
                        std::vector< RunLengthCodeBinary > _codes;
                        std::vector<int> _idx;
                public:
                        RunLengthObject ( const VolumeData<T>& data )
                        {
                                auto& codes = this->_codes;
                                auto& idx   = this->_idx;
                                const auto& size = data.getSize();
                                idx.reserve ( size.y() * size.z() + 1 );

                                for ( auto z = 0 ; z < size.z() ; ++z ) {
                                        for ( auto y = 0 ; y < size.y() ; ++y ) {
                                                idx.push_back ( static_cast<int> ( codes.size() ) );
                                                auto start = 0;
                                                auto value0 = data.get ( start, y, z );
                                                short int length = 1;

                                                for ( auto x = 1 ; x < size.x() ; ++x ) {
                                                        const auto value1 = data.get ( x, y, z );

                                                        if ( value0 == value1 ) {
                                                                ++length;
                                                        } else {
                                                                if ( value0 != 0 ) {
                                                                        codes.emplace_back ( RunLengthCodeBinary  ( mi4::Vector3s ( start, y, z ), length ) );
                                                                }

                                                                start = x;
                                                                value0 = value1;
                                                                length = 1;
                                                        }
                                                }

                                                // if the run length does not stop, top
                                                if ( value0 != 0 ) {
                                                        codes.emplace_back ( RunLengthCodeBinary (  mi4::Vector3s ( start, y, z ), length ) );
                                                }
                                        }
                                }

                                idx.push_back ( static_cast<int> ( codes.size() ) );
                                return;
                        }

                        int num_label ( void ) const
                        {
                                return this->_idx.back();
                        }

                        std::vector<RunLengthCodeBinary>& codes ( void )
                        {
                                return this->_codes;
                        }

                        std::vector<int>& index ( void )
                        {
                                return this->_idx;
                        }
                };
        private:
                const mi4::Point3i _size;
                RunLengthObject<char> _rlo;
                std::vector<id_t> _labels;
                int _num_label;
        public:
                ConnectedComponentLabeller ( const mi4::VolumeData<char>& data ) : _size ( data.getSize() ), _rlo ( RunLengthObject<char> ( data ) )
                {
                        return;
                }
                int label ( const bool isSorted = false, const bool joinXyz = true )
                {
                        const auto& size = this->_size;
                        auto& codes = this->_rlo.codes();

                        // join in xy-plane
                        for ( auto z = 0 ; z < size.z() ; ++z ) {
                                for ( auto y = 0 ; y < size.y() - 1 ; ++y ) {
                                        this->join_xy ( z, y );
                                }
                        }

                        if ( joinXyz ) {
                                for ( auto z = 0 ; z < size.z() - 1 ; ++z ) {
                                        this->join_xyz ( z );
                                }
                        }

                        // now all connected components are recognized as "connected".
                        // reduce the number of labels.
                        this->_labels.assign ( codes.size(), 0 );
                        std::vector<int> voxelCount;
                        voxelCount.push_back ( 0 ); // for background .
                        int count = 1;

                        for ( size_t i = 0 ; i < codes.size() ; ++i ) {
                                const int parent = this->get_parent ( static_cast<int> ( i ) );

                                if ( parent == static_cast<int> ( i ) ) {
                                        this->_labels[i] = count;
                                        voxelCount.push_back ( codes[i].getLength() );
                                        ++count;
                                } else {
                                        const auto newLabel = this->_labels[ parent ];
                                        this->_labels[i] = newLabel;
                                        voxelCount [ newLabel ] += codes[i].getLength();
                                }
                        }

                        // Sort labels by descend order
                        if ( isSorted ) {
                                std::vector<std::pair<int, id_t> > pairs;

                                for ( size_t i = 0 ; i < voxelCount.size() ; ++i ) {
                                        pairs.push_back ( std::make_pair ( - voxelCount[i], static_cast<id_t> ( i ) ) );
                                }

                                std::sort ( pairs.begin() + 1, pairs.end() );

                                for ( size_t i = 0 ; i < voxelCount.size() ; ++i ) {
                                        pairs[i].first = pairs[i].second;
                                        pairs[i].second = static_cast<id_t> ( i );
                                }

                                std::sort ( pairs.begin() + 1, pairs.end() );

                                for ( size_t i = 0 ; i < codes.size() ; ++i ) {
                                        this->_labels[i] = pairs[ this->_labels[i] ].second;
                                }

                                voxelCount.clear();
                                voxelCount.assign ( pairs.size(), 0 ) ;

                                for ( size_t i = 0 ; i < codes.size() ; ++i ) {
                                        voxelCount [ this->_labels[i] ] += codes[i].getLength();
                                }

                        }

                        return count - 1 ;

                }
                template <typename T> bool getData ( VolumeData<T>& labelData )
                {
                        auto& codes = this->_rlo.codes();

                        for ( size_t i = 0 ; i < codes.size() ; ++i ) {
                                this->set_label ( labelData, codes[i], this->_labels[i] );
                        }

                        return true;
                }
                template <typename T> bool getNthComponent ( VolumeData<T>& labelData, const id_t n = 1 )
                {
                        auto& codes = this->_rlo.codes();

                        for ( size_t i = 0 ; i < codes.size() ; ++i ) {
                                if ( n != this->_labels[i] ) {
                                        continue;
                                }

                                this->set_label ( labelData, codes[i], 1 );
                        }

                        return true;

                }

                template <typename T>
                bool getNotNthComponents ( VolumeData<T>& labelData, const id_t n = 1 )
                {
                        auto& codes = this->_rlo.codes();

                        for ( size_t i = 0 ; i < codes.size() ; ++i ) {
                                if ( n == this->_labels[i] ) {
                                        continue;
                                }

                                this->set_label ( labelData, codes[i], 1 );
                        }

                        return true;
                }
        private:
                template<typename T>
                void set_label ( VolumeData<T>& labelData, RunLengthCodeBinary& code, const int label )
                {
                        const auto& start = code.getStart();
                        const auto length = code.getLength();

                        for ( auto j = 0 ; j < length ; ++j ) {
                                labelData.set ( start.x() + j, start.y(), start.z(), label );
                        }
                }


                void join_xyz ( const int z )
                {
                        const auto& idx = this->_rlo.index();
                        const auto& size = this->_size;
                        const auto id0 =  z * size.y() ;
                        const auto id1 =  ( z + 1 ) * size.y() ;

                        for ( auto i = 0 ; i < size.y() ; ++i ) {
                                for ( auto j =  -1 ;  j <= 1 ; ++j ) {
                                        if ( i + j < 0 ) {
                                                continue;
                                        }

                                        if ( i + j >= size.y() ) {
                                                continue;
                                        }

                                        for ( auto k = idx [ id0 + i ] ; k < idx[id0 + i + 1] ; ++k ) {
                                                for ( auto l = idx [ id1 + i + j ] ; l < idx[id1 + i + j + 1] ; ++l ) {
                                                        this->connect ( k, l );
                                                }
                                        }
                                }
                        }
                }
                void join_xy ( const int z, const int y )
                {
                        const auto& idx = this->_rlo.index();
                        const auto& size = this->_size;
                        const auto  id0 =  z * size.y() + y ;

                        for ( auto i = idx[ id0 ] ; i < idx[id0 + 1] ; ++i ) {
                                for ( auto j = idx[id0 + 1] ; j < idx[id0 + 2] ; ++j ) {
                                        this->connect ( i, j );
                                }
                        }
                }
                int get_parent ( const id_t id )
                {
                        const auto parent = this->_rlo.codes()[id].getParent();
                        return ( parent == -1 ) ? id : this->get_parent ( parent );
                }

                // p: parent , c : child
                void set_parent ( const id_t p, const id_t c )
                {
                        this->_rlo.codes().at ( c ).setParent ( p );
                        this->_num_label -= 1;
                }

                void connect ( const id_t i, const id_t j )
                {
                        auto& codes = this->_rlo.codes();

                        if ( codes[i].isConnected ( codes[j] ) ) {
                                const auto p0 = this->get_parent ( i );
                                const auto p1 = this->get_parent ( j );

                                // both parent
                                if ( p0 == -1 && p1 == -1 ) {
                                        if ( i < j ) {
                                                this->set_parent ( i, j );
                                        } else {
                                                this->set_parent ( j, i );
                                        }
                                }
                                // both child : select lower
                                else if ( p0 != -1 && p1 != -1 )  {
                                        if      ( p0 < p1 ) {
                                                this->set_parent ( p0, p1 );
                                        } else if ( p0 > p1 ) {
                                                this->set_parent ( p1, p0 );
                                        }
                                } else {
                                        if      ( p0 == -1 ) {
                                                this->set_parent ( p1, i );
                                        } else if ( p1 == -1 ) {
                                                this->set_parent ( p0, j );
                                        }
                                }
                        }
                }
        };
}
#endif //MI4_CONNECTED_COMPONENT_LABELLER_RLE_HPP
