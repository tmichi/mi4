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
                Normalizer ( const Eigen::AlignedBox3d& gbox, const Eigen::AlignedBox3d& lbox = Eigen::AlignedBox3d ( Eigen::Vector3d ( 0, 0, 0 ), Eigen::Vector3d ( 1, 1, 1 ) ) )
                {
                        const auto v0 = this->avoid_zero ( gbox.max() - gbox.min() );
                        const auto v1 = this->avoid_zero ( lbox.max() - lbox.min() );
                        this->_mat = Eigen::Translation3d ( lbox.min() )
                                     * Eigen::Scaling ( v1.x() / v0.x(), v1.y() / v0.y(), v1.z() / v0.z() )
                                     * Eigen::Translation3d ( -gbox.min() );
                        this->_inv = this->_mat.inverse();
                        return;
                }
                Normalizer ( const Eigen::Vector3d& bmin,
                             const Eigen::Vector3d& bmax,
                             const Eigen::Vector3d& lmin = Eigen::Vector3d ( 0, 0, 0 ),
                             const Eigen::Vector3d& lmax = Eigen::Vector3d ( 1, 1, 1 ) )
                {
                        const auto v0 = this->avoid_zero ( bmax - bmin );
                        const auto v1 = this->avoid_zero ( lmax - lmin );
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

        private:
                // avoid zero-denominator.
                inline Eigen::Vector3d avoid_zero ( const Eigen::Vector3d& v ) const
                {
                        Eigen::Vector3d result;
                        result.x() = this->check_zero ( v.x() );
                        result.y() = this->check_zero ( v.y() );
                        result.z() = this->check_zero ( v.z() );
                        return result;
                }

                inline double check_zero ( const double& v ) const
                {
                        return ( v < 1.0e-40 ) ? 1 : v;
                }
        };
}
#endif
