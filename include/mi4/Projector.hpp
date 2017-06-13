/**
 * @file Projector.hpp
 * @author Takashi Michikawa <michikawa@acm.org>
 */

#ifndef MI_PROJECTOR_HPP
#define MI_PROJECTOR_HPP 1
#include <array>
#include <memory>

#include <Eigen/Dense>

namespace mi4
{
        class Projector
        {
        public:
                explicit Projector ( double modelview[16], double projection[16], int viewport[4] )
                {
                        this->_matrix =  Eigen::Map<Eigen::Matrix4d> ( projection ) * Eigen::Map<Eigen::Matrix4d> ( modelview );
                        this->_inv_matrix = this->_matrix.inverse();

                        for ( int i = 0 ; i < 4 ; ++i ) {
                                this->_viewport[i] = viewport[i];
                        }

                }
                explicit Projector ( const Eigen::Matrix4d& modelview, const Eigen::Matrix4d&  projection, int viewport[4] )
                {
                        this->_matrix =   projection  *  modelview ;
                        this->_inv_matrix = this->_matrix.inverse();

                        for ( int i = 0 ; i < 4 ; ++i ) {
                                this->_viewport[i] = viewport[i];
                        }

                }
                ~Projector ( void ) = default;

                Eigen::Vector2d project ( const Eigen::Vector3d& p, double* depth )
                {
                        Eigen::Matrix4d&  matrix = this->_matrix;
                        std::array<int, 4>& vp = this->_viewport;
                        Eigen::Vector4d p0 = matrix * Eigen::Vector4d ( p.x(), p.y(), p.z(), 1 );

                        if ( p0.w() == 0 ) {
                                return Eigen::Vector2d();
                        }

                        p0.x() *= 1.0 / p0.w();
                        p0.y() *= 1.0 / p0.w();
                        p0.z() *= 1.0 / p0.w();
                        *depth = ( 1.0 + p0.z() ) * 0.5;
                        return Eigen::Vector2d ( vp[0] + ( 1 + p0.x() ) * vp[2] * 0.5,
                                                 vp[1] + ( 1 + p0.y() ) * vp[3] * 0.5 );
                }

                Eigen::Vector3d unproject ( const Eigen::Vector2d& wp, double depth )
                {
                        Eigen::Vector3d unPrjP;

                        Eigen::Matrix4d&  inv_matrix = this->_inv_matrix;
                        std::array<int, 4>& vp = this->_viewport;
                        Eigen::Vector4d p;
                        p.x() = ( wp.x() - vp[0] ) * 2 / vp[2] - 1.0;
                        p.y() = ( wp.y() - vp[1] ) * 2 / vp[3] - 1.0;
                        p.z() = 2.0 * depth  - 1.0;
                        p.w() = 1.0;

                        const Eigen::Vector4d p1 = inv_matrix * p;

                        if ( p1.w() != 0.0 ) {
                                unProj = 1.0 / p1.w() *  Eigen::Vector3d(p1.x(), p1.y(), p1.z());
                        /*        unPrjP.x() = p1.x() / p1.w();
                                unPrjP.y() = p1.y() / p1.w();
                                unPrjP.z() = p1.z() / p1.w(); */
                        }

                        return unPrjP;
                }
        private:
                Eigen::Matrix4d _matrix;
                Eigen::Matrix4d _inv_matrix;
                std::array<int, 4>  _viewport;
        };
}

#endif// MI_PROJECTOR_HPP
