/// @file Kdtree.hpp
/// @author Takashi Michikawa <michikawa@acm.org>
#ifndef MI4_KDTREE_HPP
#define MI4_KDTREE_HPP 1

#include <cmath>
#include <vector>
#include <list>
#include <algorithm>
#include <memory>

namespace mi4
{
        /// @class Kdtree Kdtree.hpp <mi4/Kdtree.hpp>
        /// @note You can integrate with any own vector types, but it must have following methods:
        /// @li T::operator[](int); to access each coordinate value.
        /// @li T::T(const T& d); copy constructor
        template < typename T, uint8_t Dim = 3 >
        class Kdtree
        {
        private:
                using vecIter = typename std::vector< T >::iterator;
                class Node
                {
                private:
                        uint8_t _dimension; // X (0), Y (1) or Z (2) .... leaf(0xff)
                        std::unique_ptr< Node[] > _children;
                        std::list< T > _points;
                private:
                        Node (const Node& that) = delete;
                        void operator = (const Node& that) = delete;
                        Node (Node&& that) = delete;
                        void operator = (Node&& that) = delete;
                public:
                        Node (void) : _dimension(0xff)
                        {
                        }

                        ~Node ( void ) = default;

                        bool init (vecIter begin, vecIter end, const size_t numMaxNode)
                        {
                                auto& points = this->_points;
                                points.clear();
                                const size_t numElements = std::distance(begin, end);

                                if ( numElements <= numMaxNode ) {
                                        points.insert(points.end(), begin, end);
                                } else {
                                        this->_dimension = this->find_separation_axis(begin, end);
                                        std::sort(begin, end, [ d = this->dim() ] (const T& a, const T& b) {
                                                return a[d] < b[d];
                                        });
                                        auto center = begin + static_cast<size_t> ( numElements / 2 );
                                        points.push_back(*center);
                                        this->_children.reset(new Node[2]); // create

                                        if ( !this->_children ) {
                                                return false;
                                        }

                                        this->_children[0].init(begin, center, numMaxNode);
                                        this->_children[1].init(center, end, numMaxNode);
                                }

                                return true;
                        }

                        bool isLeaf ( void ) const
                        {
                                return (this->_dimension == 0xff);
                        }

                        void find (const T& p, const double radius, typename std::list< T >& result)
                        {
                                if ( this->isLeaf() ) {
                                        std::copy_if(this->_points.begin(), this->_points.end(), std::back_inserter(result), [ sqr = radius * radius, p ] (const T& x) {
                                                double d2 = 0;

                                                for ( size_t i = 0; i < Dim; i++ ) {
                                                        d2 += (x[i] - p[i]) * (x[i] - p[i]);
                                                }

                                                return d2 <= sqr;
                                        });
                                } else {
                                        if ( std::fabs(this->_points.front()[this->dim()] - p[this->dim()]) <=
                                             radius ) {
                                                this->_children[0].find(p, radius, result);
                                                this->_children[1].find(p, radius, result);
                                        } else {
                                                this->child(p).find(p, radius, result);
                                        }
                                }
                        }

                        void add (const T& p)
                        {
                                this->isLeaf() ? this->_points.push_back(p) : this->child(p).add(p);
                        };

                        std::vector< T > getAllPoints (void) const
                        {
                                std::vector< T > points;
                                this->get_all(points);
                                return points;
                        }
                private:
                        Node& child (const T& p)
                        {
                                return (p[this->dim()] < this->_points.front()[this->dim()]) ? this->_children[0] :
                                       this->_children[1];
                        }
                        uint8_t dim (void) const
                        {
                                return this->_dimension;
                        }

                        void get_all (std::vector< T >& points)
                        {
                                if ( this->isLeaf() ) {
                                        points.insert(points.end(), this->_points.begin(), this->_points.end());
                                } else {
                                        this->_children[0].getAll(points);
                                        this->_children[1].getAll(points);
                                }
                        }

                        int8_t find_separation_axis (vecIter begin, vecIter end) const
                        {
                                auto bmin = *begin, bmax = *begin;

                                for ( auto iter = begin; iter != end; ++iter ) {
                                        auto p = *iter;

                                        for ( size_t i = 0; i < Dim; ++i ) {
                                                bmin[i] = std::min(p[i], bmin[i]);
                                                bmax[i] = std::max(p[i], bmax[i]);
                                        }
                                }

                                auto dev = std::make_tuple(uint8_t(0), bmax[0] - bmin[0]);

                                for ( size_t i = 0; i < Dim; i++ ) {
                                        if ( std::get< 1 >(dev) < bmax[i] - bmin[i] ) {
                                                dev = std::make_tuple(i, bmax[i] - bmin[i]);
                                        }
                                }

                                return std::get< 0 >(dev);
                        }
                };

        private:
                Kdtree (const Kdtree& that) = delete;
                Kdtree& operator = (const Kdtree& that) = delete;
                Kdtree (Kdtree&& that) = delete;
                Kdtree& operator = (Kdtree&& that) = delete;
        private:
                std::unique_ptr< Node > _parent;
                size_t _numElement;
                size_t _numMaxElementsPerNode;
        public:
                explicit Kdtree (std::vector< T >& points = std::vector< T >(), const size_t numMaxElementsPerNode = 10) : _parent(new Node), _numElement(0), _numMaxElementsPerNode(numMaxElementsPerNode)
                {
                        this->build(points);
                }

                ~Kdtree ( void ) = default;

                bool rebuild (void)
                {
                        return this->build(this->_parent->getAllPoints());
                }

                void findn (const T& p, const size_t num, std::list< T >& nodes, const double init_radius = 0.0001)
                {
                        auto r = init_radius;
                        nodes.clear();

                        while ( nodes.size() < num ) {
                                r *= nodes.empty() ? 2.0 : std::cbrt(num * 1.0f / nodes.size());
                                this->find(p, r, nodes, false);
                        }

                        this->sort_result_by_distance(p, nodes);
                        nodes.resize(num);
                }

                void find (const T& p, const double radius, std::list< T >& nodes, const bool isSorted = false)
                {
                        nodes.clear();
                        this->_parent->find(p, radius, nodes);

                        if ( isSorted ) {
                                this->sort_result_by_distance(p, nodes);
                        }
                }

                T closest (const T& p) const
                {
                        std::list< T > nodes;
                        this->findn(p, 1, nodes);
                        return nodes.front();
                }

                void add (const T& p)
                {
                        this->_parent->add(p);
                        this->_numElement += 1;
                }

                size_t size (void) const
                {
                        return this->_numElement;
                }

        private:
                bool build (std::vector< T >& point)
                {
                        this->_numElement = point.size();
                        return this->_parent->init(point.begin(), point.end(), this->_numMaxElementsPerNode);
                }
                void sort_result_by_distance (const T& p, std::list< T >& nodes)
                {
                        nodes.sort([ &p ] (const T& a, const T& b) {
                                std::tuple< double, double > x = std::make_tuple(0, 0);

                                for ( size_t i = 0; i < Dim; i++ ) {
                                        std::get< 0 >(x) += (a[i] - p[i]) * (a[i] - p[i]);
                                        std::get< 1 >(x) += (b[i] - p[i]) * (b[i] - p[i]);
                                }

                                return std::get< 0 >(x) < std::get< 1 >(x);
                        });
                }
        };

        template < typename T, typename S = int, uint8_t Dim = 3 >
        class IndexedVector : public T
        {
        private:
                S _id;
        public:
                explicit IndexedVector (const T& v = T(), const S id = -1) : T(v), _id(id) {}
                ~IndexedVector (void) = default;
                IndexedVector (const IndexedVector& that) = default;
                IndexedVector (IndexedVector&& that) = default;
                IndexedVector& operator = (const IndexedVector& that) = default;
                IndexedVector& operator = (IndexedVector&& that) = default;
                S id ( void ) const
                {
                        return this->_id;
                }
        };
}
#endif// MI4_KDTREE_HPP
