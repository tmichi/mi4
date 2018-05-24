/**
 * @file Octree.hpp
 * @author Takashi Michikawa
 */
#ifndef MI4_OCTREE_HPP
#define MI4_OCTREE_HPP 1

#include <deque>
#include <cmath>
#include <iostream>
#include <memory>
#include <fstream>

namespace mi4
{
        typedef unsigned char OctreeNodeType;
        static OctreeNodeType const OCTREE_INVALID = 0xFF;
        static OctreeNodeType const OCTREE_LEAF  = 0x02;
        static OctreeNodeType const OCTREE_EMPTY = 0x01; // LEAF and contains emptyValue : use only serialization
        static OctreeNodeType const OCTREE_INTERMEDIATE = 0x00;
        static const int NUM_CHILDREN = 8;

        class octree_non_copyable
        {
        private:
                octree_non_copyable ( const octree_non_copyable& ) = delete;
                octree_non_copyable ( octree_non_copyable&& ) = delete;

                void operator = ( const octree_non_copyable& ) = delete;
                void operator = ( octree_non_copyable&& ) = delete;
        public:
                octree_non_copyable ( void ) = default;
                virtual ~octree_non_copyable ( void ) = default;
        };

        /**
         * @class Octree Octree.hpp <mi/Octree.hpp>
         * @brief Octree implementation.
         */
        template < typename T >
        class Octree : private octree_non_copyable
        {
        private:
                class node : private octree_non_copyable
                {
                private:
                        T _value; // value
                        std::unique_ptr<node[]> _child;
                public:
                        explicit node ( const T value = T() ) : _value ( T() )
                        {
                                return;
                        }
                        ~node ( void ) = default ;

                        T get ( const int x, const int y, const int z, const int dimension ) const
                        {
                                if ( this->is_leaf() ) {
                                        return this->_value; //getter
                                } else {
                                        const int d = dimension / 2;
                                        const auto idx = this->index ( x, y, z, d );
                                        return this->child ( idx ).get ( x % d, y % d, z % d, d );
                                }
                        }

                        void set ( const int x, const int y, const int z, const T v, const int dimension )
                        {
                                if ( dimension == 1 ) {
                                        this->value() = v;
                                } else {
                                        this->create_children();
                                        const int d =  dimension / 2;
                                        const auto idx = this->index ( x, y, z, d );
                                        this->child ( idx ).set ( x % d, y % d, z % d, v, d );
                                }

                                return;
                        }


                        void set ( const int mnx, const int mny, const int mnz, const int mxx, const int mxy, const int mxz, const T v, const int dimension )
                        {
                                if ( this->is_same ( mnx, mny, mnz, 0 ) && this->is_same ( mnx, mny, mnz, dimension - 1 ) ) {
                                        this->value() = v;
                                        return;
                                } else {
                                        const int d = dimension / 2 ;
                                        this->create_children();

                                        for ( int z = 0 ; z < 2 ; ++z ) {
                                                for ( int y = 0 ; y < 2 ; ++y ) {
                                                        for ( int x = 0 ; x < 2 ; ++x ) {
                                                                const int nmnx = d * x;
                                                                const int nmny = d * y;
                                                                const int nmnz = d * z;

                                                                const int nmxx = d * x + d - 1;
                                                                const int nmxy = d * y + d - 1;
                                                                const int nmxz = d * z + d - 1;

                                                                int bminx, bminy, bminz, bmaxx, bmaxy, bmaxz;

                                                                if ( !this->overlap ( mnx, mxx, nmnx, nmxx, bminx, bmaxx ) ) {
                                                                        continue;
                                                                }

                                                                if ( !this->overlap ( mny, mxy, nmny, nmxy, bminy, bmaxy ) ) {
                                                                        continue;
                                                                }

                                                                if ( !this->overlap ( mnz, mxz, nmnz, nmxz, bminz, bmaxz ) ) {
                                                                        continue;
                                                                }

                                                                const int idx = this->index ( x, y, z, 1 );
                                                                this->child ( idx ).set ( bminx % d, bminy % d, bminz % d, bmaxx % d, bmaxy % d, bmaxz % d, v, d );
                                                        }
                                                }
                                        }

                                        return;
                                }
                        }

                        bool bounding_box ( const T emptyValue, int& mnx, int& mny, int& mnz, int& mxx, int& mxy, int& mxz, const int dimension )
                        {
                                if ( this->is_leaf() ) {
                                        if ( this->value() == emptyValue ) {
                                                return false;
                                        }

                                        mnx = mny = mnz = 0;
                                        mxx = mxy = mxz = dimension - 1;
                                        return true;
                                } else {
                                        const int d = dimension / 2 ;
                                        mnx = mny = mnz = dimension - 1;
                                        mxx = mxy = mxz = 0;

                                        for ( int z = 0 ; z < 2 ; ++z ) {
                                                for ( int y = 0 ; y < 2 ; ++y ) {
                                                        for ( int x = 0 ; x < 2 ; ++x ) {
                                                                int lnx, lny, lnz, lxx, lxy, lxz;

                                                                // skip when the child node is empty.
                                                                const auto idx = this->index ( x, y, z, 1 );

                                                                if ( !this->child ( idx ).bounding_box ( emptyValue, lnx, lny, lnz, lxx, lxy, lxz, d ) ) {
                                                                        continue;
                                                                }

                                                                const int offx = d * x;
                                                                const int offy = d * y;
                                                                const int offz = d * z;

                                                                mnx = std::min ( lnx + offx, mnx );
                                                                mny = std::min ( lny + offy, mny );
                                                                mnz = std::min ( lnz + offz, mnz );

                                                                mxx = std::max ( lxx + offx, mxx );
                                                                mxy = std::max ( lxy + offy, mxy );
                                                                mxz = std::max ( lxz + offz, mxz );
                                                        }
                                                }
                                        }

                                        return ( mnx <= mxx ) && ( mny <= mxy ) && ( mnz <= mxz ); // bounding box exists or not.
                                }
                        }

                        size_t count ( const T value, const int dimension ) const
                        {
                                size_t result = 0;

                                if ( this->is_leaf() ) {
                                        if ( this->value() == value ) {
                                                result =  dimension * dimension * dimension;
                                        }
                                } else {
                                        for ( int i = 0 ; i < NUM_CHILDREN ; ++i ) {
                                                result += this->child ( i ).count ( value, dimension / 2 );
                                        }
                                }

                                return result;
                        }

                        bool optimize ( void )
                        {
                                if ( this->is_leaf() ) {
                                        return true;
                                } else {
                                        for ( int i = 0 ; i < NUM_CHILDREN ; ++i ) {
                                                if ( !this->child ( i ).optimize() ) {
                                                        return false;        // child node cannot be optimized
                                                }

                                                if (  this->child ( 0 ).value() != this->child ( i ).value() ) {
                                                        return false;        // different node.
                                                }
                                        }

                                        this->value() = this->child ( 0 ).value();
                                        this->_child.release(); // delete node.
                                        return true;
                                }
                        }

                        bool write ( std::ofstream& fout, const T& emptyValue )
                        {
                                OctreeNodeType type;

                                if      ( !this->is_leaf() ) {
                                        type = OCTREE_INTERMEDIATE;
                                } else if ( this->value() == emptyValue ) {
                                        type = OCTREE_EMPTY;
                                } else {
                                        type = OCTREE_LEAF;
                                }

                                if ( ! fout.write ( ( char* ) ( &type ), sizeof ( OctreeNodeType ) ) ) {
                                        return false;
                                }

                                if ( type == OCTREE_INTERMEDIATE ) {
                                        for ( int i = 0 ; i < NUM_CHILDREN ; ++i ) {
                                                if ( ! this->child ( i ).write ( fout, emptyValue ) ) {
                                                        return false;
                                                }
                                        }
                                } else if ( type == OCTREE_LEAF ) {
                                        return fout.write ( ( char* ) ( & ( this->value() ) ), sizeof ( T ) ).good();
                                }

                                return true;
                        }

                        bool read (  std::ifstream& fin, const T& emptyValue )
                        {
                                OctreeNodeType type;

                                if (  !fin.read ( ( char* ) ( &type ), sizeof ( OctreeNodeType ) ) ) {
                                        return false;
                                }

                                if ( type == OCTREE_EMPTY ) {
                                        this->value() = emptyValue;
                                        return true;
                                } else if ( type == OCTREE_LEAF ) {
                                        return fin.read ( ( char* ) ( &this->value() ), sizeof ( T ) ).good();
                                } else if ( type == OCTREE_INTERMEDIATE ) {
                                        this->create_children();

                                        for ( int i = 0 ; i < NUM_CHILDREN ; ++i ) {
                                                if ( !this->child ( i ).read ( fin, emptyValue ) ) {
                                                        return false;
                                                }
                                        }

                                        return true;
                                } else {
                                        std::cerr << "invalid node was found." << std::endl;
                                        return false;
                                }
                        }

                private:
                        inline T& value ( void )
                        {
                                return this->_value;
                        }

                        inline node& child ( const int idx ) const
                        {
                                return this->_child.get()[idx];
                        }


                        inline int index ( const int x, const int y, const int z, const int sep ) const
                        {
                                int idx = 0;

                                if ( sep <= x ) {
                                        idx += 1;
                                }

                                if ( sep <= y ) {
                                        idx += 2;
                                }

                                if ( sep <= z ) {
                                        idx += 4;
                                }

                                return idx;
                        }

                        inline bool overlap ( int mn0, int mx0, int mn1, int mx1, int& mn2, int& mx2 )
                        {
                                mn2 = mn0 > mn1 ? mn0 : mn1; //
                                mx2 = mx0 < mx1 ? mx0 : mx1;
                                return ( mn2 <= mx2 ) ;
                        }

                        inline bool is_leaf ( void ) const
                        {
                                return ! ( this->_child ); // the node is leaf when the child is null
                        }

                        inline void create_children ( void )
                        {
                                if ( this->is_leaf() ) {
                                        this->_child.reset ( new node[NUM_CHILDREN] );

                                        for ( int i = 0 ; i < NUM_CHILDREN ; ++i ) {
                                                this->child ( i ).value() = this->value();
                                        }
                                }

                                return;
                        }

                        bool is_same ( const int x, const int y, const int z, const int v )
                        {
                                return ( x == v ) && ( y == v ) && ( z == v ) ;
                        }
                };// node

                class Impl : private octree_non_copyable
                {
                private:
                        std::unique_ptr<node>	_root;
                        int	_dimension;
                        T	_emptyValue;
                public:
                        explicit Impl ( const int dimension, const int emptyValue ) : _root ( new node ( emptyValue ) ), _dimension ( dimension ), _emptyValue ( emptyValue )
                        {
                                return;
                        }

                        ~Impl ( void ) = default;

                        std::unique_ptr<node>& root ( void )
                        {
                                return this->_root;
                        }

                        int& dimension ( void )
                        {
                                return this->_dimension;
                        }

                        T& emptyValue ( void )
                        {
                                return this->_emptyValue;
                        }
                };

                std::unique_ptr<Impl> _impl;
        public:
                explicit Octree ( const int dimension = 1, const T emptyValue = T() ) : _impl ( new Impl ( dimension, emptyValue ) )
                {
                        return;
                }

                ~Octree ( void ) = default;

                void init (  const int dimension, const T emptyValue )
                {
                        this->_impl.reset ( new Impl ( dimension, emptyValue ) );
                        return;
                }

                T get ( const int x, const int y, const int z ) const
                {
                        return this->isValid ( x, y, z ) ? this->root()->get ( x, y, z, this->dimension() ) : this->empty_value();
                }

                void set ( const int x, const int y, const int z, const T v )
                {
                        if ( this->isValid ( x, y, z ) ) {
                                this->root()->set ( x, y, z, v, this->dimension() );
                        }

                        return;
                }

                void set ( const int mnx, const int mny, const int mnz, const int mxx, const int mxy, const int mxz, const T v )
                {
                        if ( this->isValid ( mnx, mny, mnz ) && this->isValid ( mxx, mxy, mxz ) ) {
                                this->root()->set ( mnx, mny, mnz, mxx, mxy, mxz, v, this->dimension() );
                        }

                        return;
                }

                inline bool isValid ( const int x, const int y, const int z ) const
                {
                        return  this->is_valid ( x ) && this->is_valid ( y ) && this->is_valid ( z ) ;
                }

                inline bool isEmpty ( const int x, const int y, const int z )
                {
                        return ( this->get ( x, y, z ) == this->empty_value() );
                }

                void getBoundingBox ( int& mnx, int& mny, int& mnz, int& mxx, int& mxy, int& mxz )
                {
                        mnx = mny = mnz = 0;
                        mxx = mxy = mxz = this->dimension() - 1;
                        this->root()->bounding_box ( this->empty_value(), mnx, mny, mnz, mxx, mxy, mxz, this->dimension() );
                        return;
                }

                inline size_t count ( const T value ) const
                {
                        return this->root()->count ( value, this->dimension() );
                }

                void optimize ( void )
                {
                        this->root()->optimize();
                        return;
                }

                bool read ( std::ifstream& fin )
                {
                        int dim;
                        T empty;

                        if ( !fin.read ( ( char* ) ( &dim ), sizeof ( int ) ) ) {
                                return false;
                        }

                        if ( !fin.read ( ( char* ) ( &empty ), sizeof ( T ) ) ) {
                                return false;
                        }

                        this->init ( dim, empty );
                        return this->root()->read ( fin, this->empty_value() );
                }

                bool write ( std::ofstream& fout )
                {
                        if ( !fout.write ( ( char* ) & ( this->dimension() ), sizeof ( int ) ) ) {
                                return false;
                        }

                        if ( !fout.write ( ( char* ) & ( this->empty_value() ), sizeof ( T ) ) ) {
                                return false;
                        }

                        return this->root()->write ( fout, this->empty_value() );
                }
        private:
                inline bool is_valid ( const int v ) const
                {
                        return 0 <= v && v < this->dimension();
                }
                inline std::unique_ptr<node>& root ( void ) const
                {
                        return this->_impl->root();
                }
                inline int& dimension ( void )
                {
                        return this->_impl->dimension();
                }
                inline int dimension ( void ) const
                {
                        return this->_impl->dimension();
                }
                inline T& empty_value ( void )
                {
                        return this->_impl->emptyValue();
                }
                inline T empty_value ( void ) const
                {
                        return this->_impl->emptyValue();
                }
        };// Octree
};
#endif// MI_OCTREE_HPP
