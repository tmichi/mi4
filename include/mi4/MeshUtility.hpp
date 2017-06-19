/**
 * @file MeshUtility.hpp
 * @author Takashi Michikawa <michikawa@acm.org>
 */
#ifndef MI_MESH_UTILITY_HPP
#define MI_MESH_UTILITY_HPP 1
#include <iostream>
#include <string>

#include "Mesh.hpp"
/*
#include "MeshExporterStl.hpp"
#include "MeshExporterPly.hpp"
#include "MeshExporterOff.hpp"
#include "MeshExporterObj.hpp"
#include "MeshImporterStl.hpp"
#include "MeshImporterObj.hpp"
#include "MeshStitcher.hpp"
*/
//#include "MeshConnectedComponentExtractor.hpp"
//#include "FileNameConverter.hpp"

namespace mi4
{
        /**
         * @class MeshUtility MeshUtility.hpp <mi/MeshUtility.hpp>
         * @brief MeshUtility methods.
         */
        class MeshUtility
        {
        private:
                MeshUtility( const MeshUtility& that ) = delete;
                void operator = ( const MeshUtility& that ) = delete;
        public:
		static bool savePly(const Mesh& mesh , const std::string& filename, const bool isBinary = true ) {
			std::ofstream fout;
			if ( isBinary ) fout.open (filename.c_str(), std::ios::binary);
			else  fout.open (filename.c_str());
			if( !fout ) return false;
                        fout<<"ply"<<std::endl;
                        if ( isBinary ) fout<<"format binary_little_endian 1.0"<<std::endl;
                        else fout<<"format ascii 1.0"<<std::endl;
                        fout<<"element vertex "<<mesh.getNumVertices()<<std::endl;
                        fout<<"property float x"<<std::endl;
                        fout<<"property float y"<<std::endl;
                        fout<<"property float z"<<std::endl;
                        fout<<"element face "<<mesh.getNumFaces()<<std::endl;
                        fout<<"property list uchar int vertex_index"<<std::endl;
                        fout<<"end_header"<<std::endl;
			for ( int i = 0 ; i < mesh.getNumVertices() ; ++i ) {
                                Eigen::Vector3d v = mesh.getPosition( i );
				if ( isBinary ) {
					float p[3];
					p[0] = static_cast<float>( v.x() );
					p[1] = static_cast<float>( v.y() );
					p[2] = static_cast<float>( v.z() );
					if( !fout.write( ( char* )( &p[0] ), sizeof( float ) * 3 ) ) return false;
				} else {
					fout<<v.x()<<" "<<v.y()<<" "<<v.z()<<std::endl;
				}
                        }
                        for ( int i = 0 ; i < mesh.getNumFaces() ; ++i ) {
                                std::vector<int> idx = mesh.getFaceIndices( i );
				if ( isBinary ) {
					unsigned char n = static_cast<unsigned char>( idx.size() );
					if( !fout.write( ( char* )( &n ), sizeof( unsigned char ) ) ) {
						return false;
					}
					if( !fout.write( ( char* )( &idx[0] ), sizeof( int ) * n ) ) {
						return false;
					}
				} else {
					fout<<idx.size();
					for ( size_t j = 0 ; j < idx.size() ; ++j ) {
						fout<<" "<<idx[idx.size() - 1 - j];
					}
					fout<<std::endl;
				}
                        }
			fout.close();
                        return true;
		}

                /**
                 * @brief Open mesh data.
                 * @param [in] mesh Mesh.
                 * @param [in] filename File name.
                 * @retval true Success.
                 * @retval false Failure.
                 */
		/*   static bool open ( Mesh& mesh, const std::string& filename, const bool isBinary = true ) {
                        std::string extension = FileNameConverter( filename ).getExtension();
                        if ( extension.compare( "stl" ) == 0 ) {
                                return MeshImporterStl( mesh ).read( filename );
                        } else if ( extension.compare( "obj" ) == 0 ) {
                                return MeshImporterObj( mesh ).read( filename );
                        } else {
                                std::cerr<<"unsupported format "<<extension<<std::endl;
                                return false;
                        }
			}*/
                /**
                 * @brief Save mesh data.
                 * @param [in] mesh Mesh.
                 * @param [in] filename File name.
                 * @retval true Success.
                 * @retval false Failure.
                 */
/*
                static bool save ( const Mesh& mesh, const std::string& filename, const bool isBinary = true ) {
                        std::string extension = FileNameConverter( filename ).getExtension();
                        if ( extension.compare( "stl" ) == 0 ) {
                                return MeshExporterStl( mesh ).write( filename );
                        } else if ( extension.compare( "ply" ) == 0 ) {
                                return MeshExporterPly( mesh , isBinary ).write( filename );
                        } else if ( extension.compare( "obj" ) == 0 ) {
                                return MeshExporterObj( mesh ).write( filename );
                        } else if ( extension.compare( "off" ) == 0 ) {
                                return MeshExporterOff( mesh ).write( filename );
                        } else {
                                std::cerr<<"unsupported format "<<extension<<std::endl;
                                return false;
                        }
                }
*/
		/**
		 * @brief Stitch a triangle soup.
		 * @param [in, out] mesh Mesh object.
		 * @param [in] eps Maximum distance to connect vertices.
		 */
		/*              static bool stitch ( Mesh& mesh, const double eps = 1.0e-10 ) {
                        Mesh mesh0;
                        MeshStitcher stitcher( mesh, eps );
                        if ( !stitcher.stitch( mesh0 ) ) return false;
			// Copying from mesh0 to mesh.
                        mesh.init();
                        for ( int i = 0 ; i < mesh0.getNumVertices(); ++i ) {
                                mi4::Vector3d p = mesh0.getPosition( i );
                                mesh.addPoint( p );
                        }
                        for ( int i = 0 ; i < mesh0.getNumFaces(); ++i ) {
                                mesh.addFace( mesh0.getFaceIndices( i ) );
                        }
                        return true;
                }
		*/
                /**
                 * @brief decompose mesh into connected components.
                 * @param [in] mesh Mesh object.
		 * @param [out] assy Assembly mesh object.
                 */
/*                static bool decompose ( const Mesh& mesh, AssemblyMesh &assy ) {
                        MeshConnectedComponentExtractor extractor( mesh );
                        extractor.extract( assy ) ;
                        return true;
                }
*/
        };
}
#endif // MI_UTILITY_HPP









