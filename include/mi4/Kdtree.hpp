/**
 * @file Kdtree.hpp
 * @author Takashi Michikawa <michikawa@acm.org>
 */
#ifndef MI_KDTREE_HPP
#define MI_KDTREE_HPP 1

#include <cmath>
#include <cstdlib>
#include <vector>
#include <list>
#include <algorithm>
#include <memory>
namespace mi4
{
        /**
        * @class Kdtree Kdtree.hpp "mi/Kdtree.hpp"
        * @note You can integrate with any own vector types, but it must have  following methods:
        * @li T::operator[](int); to access each coordinate value.
        * @li T::T(const T& d); copy constructor
        */
        template <typename T, size_t Dim = 3>
        class Kdtree
        {
        private:
                class Node
                {
                private:
                        char		_dimension; // X (0), Y (1) or Z (2) ....
                        std::unique_ptr<Node[]> _child;
                        std::list<T> 	_points;
                private:
                        Node(const Node &that) = delete;

                        Node(Node &&that) = delete;

                        void operator=(const Node &that) = delete;

                        void operator=(Node &&that) = delete;
                public:
                        Node ( void ) : _dimension ( -1 )
                        {
                                return;
                        }
                        ~Node ( void ) = default;

                        bool init ( typename std::vector<T>::iterator begin, typename std::vector<T>::iterator end, const size_t numMaxNode )
                        {
                                auto &child = this->_child;
                                auto &points = this->_points;
                                auto &dim = this->_dimension;
                                points.clear();

                                const size_t numElements = static_cast<size_t>(std::distance(begin, end));
                                if (numElements <= numMaxNode) {
                                        points.insert(points.end(), begin, end);
                                } else {
                                        dim = this->find_separation_axis(begin, end);

                                        std::sort(begin, end,
                                                  [dim](const T &a, const T &b) { return a[dim] < b[dim]; });
                                        auto center = begin + static_cast<size_t> ( numElements / 2 );
                                        points.push_back(*center);

                                        child.reset(new Node[2]); // create
                                        if (!child) return false;
                                        child[0].init(begin, center, numMaxNode);
                                        child[1].init(center, end, numMaxNode);
                                }
                                return true;
                        }

                        bool isLeaf ( void ) const
                        {
                                return ( this->_dimension == -1 );
                        }

                        void find ( const T& pnt, const double radius, typename std::list<T>& result )
                        {
                                auto &points = this->_points;
                                auto &child = this->_child;
                                if (points.empty()) {
                                        return;
                                }

                                if ( this->isLeaf() ) {
                                        const auto sqr = radius * radius;

                                        for (const auto &iter : points) {
                                                double squaredDistance = 0;

                                                // ||iter - pnt ||^2 <= radius^2
                                                for ( size_t i = 0 ; i < Dim ; i++ ) {
                                                        squaredDistance += ( iter[i] - pnt[i] ) * ( iter[i] - pnt[i] );
                                                }
                                                if  ( squaredDistance <= sqr ) {
                                                        result.push_back ( iter );
                                                }
                                        }
                                } else {
                                        const auto& d = this->_dimension;
                                        const auto p = points.front()[d]; // separator
                                        const auto x = pnt[d]; // target

                                        if ( fabs ( x - p ) <= radius ) {
                                                child[0].find(pnt, radius, result);
                                                child[1].find(pnt, radius, result);
                                        } else {
                                                if (x < p) child[0].find(pnt, radius, result);
                                                else child[1].find(pnt, radius, result);
                                        }
                                }

                                return;
                        }

                        void add ( const T& p, const size_t numMaxElementsPerNode )
                        {
                                auto& points = this->_points;
                                const auto& d = this->_dimension;

                                if ( this->isLeaf() ) {
                                        points.push_back ( p );

                                        if ( points.size() > numMaxElementsPerNode ) {
                                                std::vector< T > points0(points.begin(), points.end());
                                                points.clear();
                                                this->init ( points0.begin(), points0.end(), numMaxElementsPerNode );
                                                points0.clear();
                                        }
                                } else {
                                        const auto delim = points.front[d];
                                        const auto x = p[d];
                                        const int i = (x < delim) ? 0 : 1;
                                        this->_child[i].add(p, numMaxElementsPerNode);
                                };

                                return;
                        };


                        void getAll ( std::vector<T>& point )
                        {
                                if ( this->isLeaf() ) {
                                        point.insert ( point.end(), this->_points.begin(), this->_points.end() );
                                } else {
                                        this->_child[0].getAll ( point );
                                        this->_child[1].getAll ( point );
                                }
                                return;
                        }
                private:
                        char find_separation_axis ( typename std::vector<T>::iterator begin, typename std::vector<T>::iterator end ) const
                        {
                                auto bmin = *begin, bmax = *begin;
                                for ( auto iter = begin ; iter != end ; ++iter ) {
                                        auto p = *iter;

                                        for (size_t i = 0; i < Dim; ++i) {
                                                bmin[i] = std::min(p[i], bmin[i]);
                                                bmax[i] = std::max(p[i], bmax[i]);
                                        }
                                }

                                char maxDim = 0;
                                double maxDeviation = bmax[0] - bmin[0];

                                for ( size_t i = 0 ; i < Dim ; i++ ) {
                                        const double deviation = bmax[i] - bmin[i];

                                        if ( maxDeviation < deviation ) {
                                                maxDim = static_cast<char> ( i );
                                                maxDeviation = deviation;
                                        }
                                }

                                return maxDim;
                        }
                };
        private:
                Kdtree(const Kdtree &that) = delete;

                Kdtree &operator=(const Kdtree &that) = delete;

                Kdtree(Kdtree &&that) = delete;

                Kdtree &operator=(Kdtree &&that) = delete;
        private:
                size_t  _numElement;
                size_t  _numMaxElementsPerNode;
                Node _parent;
                double _init_radius;
        public:
                explicit Kdtree(std::vector< T > &point = std::vector< T >(), const size_t numMaxElementsPerNode = 10,
                                const double init_radius = 0.001) : _numElement(0)
                {
                        this->build ( point, numMaxElementsPerNode, init_radius );
                        return;
                }

                ~Kdtree ( void ) = default;

                bool build ( std::vector<T>& point, const size_t numMaxElementsPerNode = 10, const double init_radius = 0.001 )
                {
                        this->_numMaxElementsPerNode = numMaxElementsPerNode;
                        this->_numElement = point.size();
                        this->_init_radius = init_radius;
                        return this->_parent.init ( point.begin(), point.end(), numMaxElementsPerNode );
                }

                bool rebuild ( const size_t numMaxElementsPerNode = 10 )
                {
                        std::vector<T> point;
                        this->_parent.getAll ( point );
                        return this->build ( point, numMaxElementsPerNode );
                }

                std::list<T> find ( const T p, const double radius, bool isSorted = false )
                {
                        std::list<T> result;
                        this->find ( p, radius, result, isSorted );
                        return result;
                }

                void find(const T p, const double radius, std::vector< T > &nodes, bool isSorted = false)
                {
                        nodes.clear();
                        std::list<T> result;
                        this->find ( p, radius, result, isSorted );
                        nodes.insert(nodes.end(), result.begin(), result.end());
                        return;
                }

                void find ( const T p, const double radius, std::list<T>& node, bool isSorted = false )
                {
                        node.clear();
                        this->_parent.find ( p, radius, node );
                        if (isSorted) this->sort_result(p, node);
                        return;
                }

                std::list<T> find ( const T p, const size_t n, bool isSorted = false )
                {
                        std::list<T> result;
                        this->find ( p, n, result, isSorted );
                        return result;
                }

                void find(const T p, const size_t n, std::vector< T > &nodes)
                {
                        nodes.clear();
                        std::list<T> result;
                        this->find(p, n, result);
                        nodes.insert ( nodes.end(), result.begin(), result.end() );
                        return;
                }

                void find(const T p, const size_t num, std::list< T > &node)
                {
                        double r = this->_init_radius;
                        const double base = std::pow ( static_cast<double> ( num ), 1.0 / 3.0 );
                        node.clear();

                        if ( this->size () < num ) {
                                std::vector<T> all;
                                this->_parent.getAll ( all );
                                node.insert ( node.end(), all.begin(), all.end() );
                        } else {
                                while ( node.size() < num ) {
                                        this->find ( p, r, node, false );
                                        r *= node.empty() ? 2 : static_cast<double> ( base * 1.0f /
                                                                                      std::pow(node.size(),
                                                                                               1.0f / 3.0f));
                                }
                                this->sort_result(p, node);
                                node.resize ( num );
                        }

                        return;
                }

                T closest ( const T& p )
                {
                        std::list< T > nodes;
                        this->find(p, 1, nodes);
                        return nodes.front();
                }

                void add ( const T& element )
                {
                        this->_parent.add ( element, this->_numMaxElementsPerNode );
                        this->_numElement += 1;
                        return;
                }

                size_t size(void) const {
                        return this->_numElement;
                }
        private:
                void sort_result(const T &p, std::list< T > &nodes) {
                        nodes.sort([&p](const T &a, const T &b) {
                                double ra = 0;
                                double rb = 0;
                                for (size_t i = 0; i < Dim; i++) {
                                        ra += (a[i] - p[i]) * (a[i] - p[i]);
                                        rb += (b[i] - p[i]) * (b[i] - p[i]);
                                }
                                return ra < rb;
                        });
                }
        };

        template <typename T, typename S = int, size_t Dim = 3>
        class IndexedVector : public T
        {
        private:
                S _id;
        public:
                explicit IndexedVector(const T &v = T(), const S id = -1) : T(v), _id(id)
                {
                        return;
                }

                ~IndexedVector() = default;

                IndexedVector(const IndexedVector< T, S, Dim > &that) = default;

                IndexedVector(IndexedVector< T, S, Dim > &&that) = default;
                IndexedVector& operator = ( const IndexedVector<T, S, Dim>& that ) = default;

                IndexedVector &operator=(IndexedVector< T, S, Dim > &&that)      = default;

                S id ( void ) const
                {
                        return this->_id;
                }
        };
}
#endif// MI_KDTREE_HPP
