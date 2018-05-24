/**
 * @file Mesh.hpp
 * @brief
 */
#ifndef MI4_MESH_HPP
#define MI4_MESH_HPP 1
#include <iostream>
#include <vector>
#include <utility>
#include <deque>
#include <map>
#include <cmath>
#include <Eigen/Dense>
namespace mi4
{
        class Mesh
        {
        private:
                std::string _name;
                std::vector<Eigen::Vector3d> _vertex; /// vertex position
                std::vector<size_t> _index; /// triangles
        public:
                void clone ( const Mesh& mesh )
                {
                        this->init();

                        for ( int i = 0 ; i < mesh.getNumVertices() ; ++i ) {
                                this->addPoint ( mesh.getPosition ( i ) );
                        }

                        for ( int i = 0 ; i < mesh.getNumFaces() ; ++i ) {

                                this->addFace ( mesh.getFaceIndices ( i ) );
                        }
                }

                size_t addPoint ( const Eigen::Vector3d& p ) const
                {
                        const_cast<Mesh*> ( this )->_vertex.push_back ( p );
                        return this->_vertex.size() - 1;
                }

                size_t addFace ( const std::vector<size_t>& fidx )
                {
                        if ( fidx.size() != 3 ) {
                                std::cerr << "only triangle is supported." << std::endl;
                                return 0;
                        }

                        this->_index.insert ( this->_index.end(), fidx.begin(), fidx.end() );
                        return this->_index.size() / 3 ; // ID
                }

                void addName ( const std::string name = std::string ( "mesh" ) )
                {
                        this->_name = name;
                        return;
                }

                inline bool isValidFaceId ( const size_t faceid ) const
                {
                        return ( faceid < this->getNumFaces() );
                }

                inline bool isValidVertexId ( const size_t vertexid ) const
                {
                        return ( vertexid < this->getNumVertices() );
                }

                inline std::vector<size_t> getFaceIndices ( const size_t faceid ) const
                {
                        std::vector<size_t> idx;

                        if ( this->isValidFaceId ( faceid ) ) {
                                for ( int i = 0 ; i < 3 ; i++ ) {
                                        idx.push_back ( this->_index.at ( faceid * 3 + i ) );
                                }
                        }

                        return idx;
                }

                inline Eigen::Vector3d getPosition ( const size_t vertexid ) const
                {
                        if ( this->isValidVertexId ( vertexid ) ) {
                                return this->_vertex.at ( vertexid );
                        }

                        return Eigen::Vector3d();
                }


                Eigen::Vector3d getNormal ( const size_t faceid, bool normalize = true ) const
                {
                        if ( ! this->isValidFaceId ( faceid ) ) {
                                return Eigen::Vector3d();
                        }

                        auto fidx = this->getFaceIndices ( faceid );
                        Eigen::Vector3d v0 = this->getPosition ( fidx[0] );
                        Eigen::Vector3d v1 = this->getPosition ( fidx[1] ) - v0;
                        Eigen::Vector3d v2 = this->getPosition ( fidx[2] ) - v0;
                        Eigen::Vector3d n = v1.cross ( v2 );

                        if ( normalize ) {
                                n.normalize();
                        }

                        return n;
                }

                inline std::string
                getName ( void ) const
                {
                        return this->_name;
                }


                void setPosition ( const size_t vertexid, const Eigen::Vector3d& pos )
                {
                        if ( this->isValidVertexId ( vertexid ) ) {
                                this->_vertex.at ( vertexid ) = pos;
                        }

                        return;
                };
                void init ( void )
                {
                        this->_vertex.clear();
                        this->_index.clear();
                        return;
                }

                inline size_t getNumVertices ( void ) const
                {
                        return this->_vertex.size();
                }

                inline size_t getNumFaces ( void ) const
                {
                        return this->_index.size() / 3;
                }

                void
                getBoundingBox ( Eigen::Vector3d& bmin, Eigen::Vector3d& bmax )
                {
                        Eigen::AlignedBox3d bbox;
                        const size_t numv = this->getNumVertices();

                        for ( size_t i = 1 ; i < numv ; ++i ) {
                                bbox.extend ( this->getPosition ( i ) );
                        }

                        bmin = bbox.min();
                        bmax = bbox.max();
                        return ;
                }

                void negateOrientation()
                {
                        for ( size_t i = 0 ; i < this->getNumFaces() ; ++i ) {
                                size_t f0 = this->_index [ i * 3 + 0 ];
                                this->_index[ i * 3 + 0 ] = this->_index [ i * 3 + 1];
                                this->_index[ i * 3 + 1 ] = f0;

                        }

                }

                double getArea ( const int faceId ) const
                {
                        std::vector<size_t> index = this->getFaceIndices ( faceId );
                        const Eigen::Vector3d p0 = this->getPosition ( index[0] );
                        const Eigen::Vector3d p1 = this->getPosition ( index[1] );
                        const Eigen::Vector3d p2 = this->getPosition ( index[2] );
                        const Eigen::Vector3d v0 = p1 - p0;
                        const Eigen::Vector3d v1 = p2 - p0;
                        return v0.cross ( v1 ).norm() * 0.5; //|v0^v1|/2
                }
        };
}//namespace mi4
#endif
