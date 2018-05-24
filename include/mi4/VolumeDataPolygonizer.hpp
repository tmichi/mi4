/**
* @file VolumeDataPolygonizer.hpp
* @author Takashi Michikawa <michikawa@acm.org>
*/
#ifndef MI4_VOLUME_DATA_POLYGONIZER_HPP
#define MI4_VOLUME_DATA_POLYGONIZER_HPP  1

#include <iostream>
#include <cmath>
#include <cstdlib>
#include <vector>

#include <mi4/VolumeData.hpp>
#include "mc_table.hpp"
#include "Mesh.hpp"

namespace mi4
{
        /**
        * @class VolumeDataPolygonizer VolumeDataPolygonizer.hpp "VolumeDataPolygonizer.hpp"
        * @brief Polygonizing volume data.
        *
        * An implementation of Marching cubes algorithm supoorting topology consistency.
        */
        template<typename T>
        class VolumeDataPolygonizer
        {

        private:
                template <typename S>
                class Data
                {
                private:
                        Data ( const Data<S>& data );
                        void operator = ( const Data<S>& data );
                private:
                        S _value;
                        mi4::VolumeData<S>* _data;
                        bool _isMonotone;
                public:
                        Data ( mi4::VolumeData<S>& data )
                        {
                                this->_data = &data;
                                this->_isMonotone = false;
                        }

                        Data ( const S value )
                        {
                                this->_value = value;
                                this->_data = NULL;
                                this->_isMonotone = true;
                        }

                        S get ( const mi4::Point3i& p )
                        {
                                if ( this->_isMonotone ) {
                                        return this->_value;
                                } else {
                                        return this->_data->get ( p );
                                }
                        }
                };
        private:
                VolumeDataPolygonizer ( const VolumeDataPolygonizer& that );
                void operator = ( const VolumeDataPolygonizer& that );
        public:
                /**
                * @brief Constructor.
                * @param [in] data Volume data.
                */
                explicit VolumeDataPolygonizer ( VolumeData<T>& data ) : _data ( data )
                {
                        this->setEps();
                        return;
                }
                /**
                * @brief Destructor.
                */
                ~VolumeDataPolygonizer ( void )
                {
                        return;
                }

                VolumeDataPolygonizer<T>& setEps ( double iso_eps = 1.0e-3 )
                {
                        this->_iso_eps = iso_eps;
                        return *this;
                }
                /**
                * @brief Polygonize the volume data.
                * @param [in] isovalue Iso value ot the volume data.
                * @param [out] mesh Mesh object.
                * @return The number of triangles.
                */
                Mesh polygonize ( const float isovalue )
                {
                        Data<float> iso ( isovalue );
                        Data<char> maskdata ( 1 );
                        return this->polygonize ( iso, maskdata );
                }


                /**
                * @brief Polygonize the volume data.
                * @param [in] isovalue Iso value ot the volume data.
                * @param [out] mesh Mesh object.
                * @return The number of triangles.
                */
                Mesh polygonize ( mi4::VolumeData<float>& isovalue )
                {
                        Data<float> iso ( isovalue );
                        Data<char> maskdata ( 1 );
                        return this->polygonize ( iso, maskdata );
                }

                /**
                * @brief Polygonize the volume data with mask.
                * @param [in] isovalue Iso value ot the volume data.
                * @param [out] mesh Mesh object.
                * @param [in] iso_eps Threashold for stitching vertices.
                * @param [in] mask Mask image
                * @return The number of triangles.
                */
                Mesh polygonize ( const float isovalue, mi4::VolumeData<char>& mask )
                {
                        if ( mask.isReadable() == false ) {
                                return Mesh();
                        }

                        Data<float> iso ( isovalue );
                        Data<char> maskdata ( mask );
                        return this->polygonize ( iso, maskdata );
                }
                /**
                * @brief Polygonize the volume data with mask.
                * @param [in] isovalue Iso value ot the volume data.
                * @param [out] mesh Mesh object.
                * @param [in] iso_eps Threashold for stitching vertices.
                * @param [in] mask Mask image
                * @return The number of triangles.
                */
                Mesh polygonize ( mi4::VolumeData<float>& isovalue, mi4::VolumeData<char>& mask )
                {
                        if ( mask.isReadable() == false ) {
                                std::cerr << "Mask unreadable" << std::endl;
                                return Mesh();
                        }

                        Data<float> iso ( isovalue );
                        Data<char> maskdata ( mask );
                        return this->polygonize ( iso, maskdata );
                }

                Mesh polygonize ( Data<float>& isovalue, Data<char>& mask )
                {
                        Mesh mesh;
                        // @todo make parallel (170619)
                        const auto& info = this->_data.getInfo();

                        for ( const auto& p : mi4::Range ( info.getMin(),  info.getMax() - mi4::Point3i ( 1, 1, 1 ) ) ) {
                                std::vector<double> iso;
                                iso.reserve ( 8 );
                                bool isPolygonized = false;
                                std::vector<std::pair<Point3d, double> > cell;
                                cell.reserve ( 8 );

                                for ( const auto& dp : mi4::Range ( mi4::Point3i ( 0, 0, 0 ), mi4::Point3i ( 1, 1, 1 ) ) ) {
                                        const Point3i np = p + dp;
                                        const Point3d pd = this->_data.getInfo().getPointInSpace ( np );
                                        const double v = this->_data.get ( np );
                                        cell.push_back ( std::make_pair ( pd, v ) );
                                        iso.push_back ( static_cast<double> ( isovalue.get ( np ) ) );

                                        if ( mask.get ( np ) > 0 ) {
                                                isPolygonized = true;
                                        }
                                }

                                if ( isPolygonized ) {
                                        this->polygonize_cell ( cell, iso, mesh );
                                }
                        }

                        return mesh;
                }
        private:
                int polygonize_cell ( std::vector< std::pair<Point3d, double> >& cell, const std::vector<double>& isovalue, Mesh& mesh )
                {
                        unsigned char tableid = 0x00;

                        for ( int i = 0 ; i < 8 ; ++i ) {
                                if ( std::fabs ( cell[i].second - isovalue[i] ) < this->_iso_eps ) {
                                        cell[i].second = isovalue[i] + this->_iso_eps ;
                                }

                                if ( isovalue[i] <=  cell[i].second ) {
                                        tableid += ( 0x01 << i );
                                }
                        }

                        if ( tableid == 0x00 || tableid == 0xFF ) {
                                return 0;
                        }

                        Vector3d ep[12];

                        for ( int i = 0 ; i < 12 ; i++ ) {
                                const int& id0 = mc_edtable[ 2 * i + 0];
                                const int& id1 = mc_edtable[ 2 * i + 1];

                                const double& v0 = cell[id0].second;
                                const double& v1 = cell[id1].second;

                                if ( ( ( tableid >> id0 ) & 0x01 )  == ( ( tableid >> id1 ) & 0x01 ) ) {
                                        continue;
                                }

                                const double& iso0 = isovalue[id0];
                                const double& iso1 = isovalue[id1];

                                double t  = - ( v0 - iso0 )  / ( ( v1 - v0 ) - ( iso1 - iso0 ) ) ;	 ///@todo check 0 division

                                const Point3d& p0 = cell[id0].first;
                                const Point3d& p1 = cell[id1].first;
                                // ep[i] = ( 1.0 - t ) * p0 + t * p1;
                                ep[i].x() = ( 1.0 - t ) * p0.x () + t * p1.x();
                                ep[i].y() = ( 1.0 - t ) * p0.y () + t * p1.y();
                                ep[i].z() = ( 1.0 - t ) * p0.z () + t * p1.z();
                        }

                        int numTriangles = 0;

                        for ( int i =  mc_colidx[tableid] ; i < mc_colidx[tableid + 1] ; i += 3 ) {
                                ++numTriangles;
                                std::vector<size_t> idx ( 3, 0 );
                                // check invert
                                idx[0] = mesh.addPoint ( ep[ mc_idxtable[i  ] ] );
                                idx[1] = mesh.addPoint ( ep[ mc_idxtable[i + 1] ] );
                                idx[2] = mesh.addPoint ( ep[ mc_idxtable[i + 2] ] );
                                mesh.addFace ( idx );
                        }

                        return numTriangles;
                }
        private:
                double         _iso_eps;
                VolumeData<T>& _data; ///< Volume data.
        };
};
#endif //
