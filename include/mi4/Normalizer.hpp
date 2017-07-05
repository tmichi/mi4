#ifndef MI4_NORMALIZER_HPP
#define MI4_NORMALIZER_HPP 1
#include <Eigen/Dense>
#include <Eigen/Geometry>

namespace mi4
{
        class Normalizer
        {
        private:
                Normalizer& operator = ( const Normalizer& that ) = delete;
                Normalizer& operator = ( Normalizer&& that ) = delete;
                Normalizer ( const Normalizer& that ) = delete;
                Normalizer ( Normalizer&& that ) = delete;
        private:
                Eigen::Affine3d _mat;
                Eigen::Affine3d _inv;
        public:
                Normalizer ( const Eigen::Vector3d& bmin, const Eigen::Vector3d& bmax,
                             const Eigen::Vector3d& lmin = Eigen::Vector3d ( 0, 0, 0 ),
                             const Eigen::Vector3d& lmax = Eigen::Vector3d ( 1, 1, 1 ) )
                {

                        Eigen::Vector3d v0 = bmax - bmin;
                        const auto v1 = lmax - lmin;

                        if ( v0.x() < 1.0e-40 ) {
                                v0.x() = 1;
                        }

                        if ( v0.y() < 1.0e-40 ) {
                                v0.y() = 1;
                        }

                        if ( v0.z() < 1.0e-40 ) {
                                v0.z() = 1;
                        }

                        this->_mat = Eigen::Translation3d ( lmin )
                                     * Eigen::Scaling ( v1.x() / v0.x(), v1.y() / v0.y(), v1.z() / v0.z() )
                                     * Eigen::Translation3d ( -bmin );
                        this->_inv = this->_mat.inverse();

                        return;
                }

                ~Normalizer ( void ) = default;

                Eigen::Vector3d normalize ( const Eigen::Vector3d& p ) const
                {
                        return this->_mat * p;
                }

                Eigen::Vector3d denormalize ( const Eigen::Vector3d& p ) const
                {
                        return this->_inv * p;
                }
        };
}
#endif
