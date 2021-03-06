/**
 * @file MeshUtility.hpp
 * @author Takashi Michikawa <michikawa@acm.org>
 */
#ifndef MI_MESH_UTILITY_HPP
#define MI_MESH_UTILITY_HPP 1
#include <iostream>
#include <string>
#include "Mesh.hpp"
#include "Kdtree.hpp"

namespace mi4
{
        /**
         * @class MeshUtility MeshUtility.hpp <mi/MeshUtility.hpp>
         * @brief MeshUtility methods.
         */
        class MeshUtility
        {
        private:
                MeshUtility ( const MeshUtility& that ) = delete;
                void operator = ( const MeshUtility& that ) = delete;
        public:
                static bool savePly ( const Mesh& mesh, const std::string& filename, const bool isBinary = true )
                {
                        std::ofstream fout;

                        if ( isBinary ) {
                                fout.open ( filename.c_str(), std::ios::binary );
                        } else {
                                fout.open ( filename.c_str() );
                        }

                        if ( !fout ) {
                                return false;
                        }

                        fout << "ply" << std::endl;

                        if ( isBinary ) {
                                fout << "format binary_little_endian 1.0" << std::endl;
                        } else {
                                fout << "format ascii 1.0" << std::endl;
                        }

                        fout << "element vertex " << mesh.getNumVertices() << std::endl;
                        fout << "property float x" << std::endl;
                        fout << "property float y" << std::endl;
                        fout << "property float z" << std::endl;
                        fout << "element face " << mesh.getNumFaces() << std::endl;
                        fout << "property list uchar int vertex_index" << std::endl;
                        fout << "end_header" << std::endl;

                        for ( size_t i = 0 ; i < mesh.getNumVertices() ; ++i ) {
                                Eigen::Vector3d v = mesh.getPosition ( i );

                                if ( isBinary ) {
                                        float p[3];
                                        p[0] = static_cast<float> ( v.x() );
                                        p[1] = static_cast<float> ( v.y() );
                                        p[2] = static_cast<float> ( v.z() );

                                        if ( !fout.write ( ( char* ) ( &p[0] ), sizeof ( float ) * 3 ) ) {
                                                return false;
                                        }
                                } else {
                                        fout << v.x() << " " << v.y() << " " << v.z() << std::endl;
                                }
                        }

                        for ( size_t i = 0 ; i < mesh.getNumFaces() ; ++i ) {
                                std::vector<size_t> idx = mesh.getFaceIndices ( i );

                                if ( isBinary ) {
                                        unsigned char n = static_cast<unsigned char> ( idx.size() );

                                        if ( !fout.write ( ( char* ) ( &n ), sizeof ( unsigned char ) ) ) {
                                                return false;
                                        }

                                        if ( !fout.write ( ( char* ) ( &idx[0] ), sizeof ( int ) * n ) ) {
                                                return false;
                                        }
                                } else {
                                        fout << idx.size();

                                        for ( size_t j = 0 ; j < idx.size() ; ++j ) {
                                                fout << " " << idx[idx.size() - 1 - j];
                                        }

                                        fout << std::endl;
                                }
                        }

                        fout.close();
                        return true;
                }
                /**
                 * @brief Stitch a triangle soup.
                 * @param [in, out] mesh Mesh object.
                 * @param [in] eps Maximum distance to connect vertices.
                 */
                static Mesh stitch ( const Mesh& mesh, const double eps = 1.0e-10 )
                {
                        Mesh resultMesh;

                        typedef IndexedVector<Vector3d, size_t> VertexType;
                        std::vector<size_t> newId ( mesh.getNumVertices(), std::numeric_limits<size_t>::max() ) ;
                        std::vector<bool> visited ( mesh.getNumVertices(), false );
                        std::vector< VertexType > points;

                        for ( size_t i = 0 ; i < mesh.getNumVertices() ; ++i ) {
                                points.push_back ( VertexType ( mesh.getPosition ( i ), i ) ) ;
                        }

                        Kdtree<VertexType> kdtree ( points );

                        for ( size_t i = 0 ; i < mesh.getNumVertices() ; ++i ) {
                                if ( visited[i] ) {
                                        continue;
                                }

                                visited[i] = true;
                                std::list<VertexType> result;
                                kdtree.find ( VertexType ( mesh.getPosition ( i ), 0 ), eps, result );
                                const size_t id = resultMesh.addPoint ( mesh.getPosition ( i ) ) ;

                                for ( auto& i : result ) {
                                        newId[ i.id() ] = id;
                                        visited[i.id()] = true;
                                }
                        }

                        for ( size_t i = 0 ; i < mesh.getNumFaces() ; ++i ) {
                                std::vector<size_t> index = mesh.getFaceIndices ( i ) ;

                                for ( size_t j = 0 ; j < index.size() ; ++j ) {
                                        const auto id = index[j];
                                        index[j] = newId [ id ];
                                }

                                resultMesh.addFace ( index );
                        }

                        return resultMesh;
                }
        };
}
#endif // MI_UTILITY_HPP







