/**
 * @file Camera.hpp
 * @author Takashi Michikawa <michikawa@acm.org>
 */
#ifndef MI4_CAMERA_HPP
#define MI4_CAMERA_HPP 1

#include <Eigen/Dense>
namespace mi4
{
        class Camera
        {
        public:
                Camera ( void ) : _rotation ( Eigen::Quaterniond ( 1, 0, 0, 0 ) ), _center ( Eigen::Vector3d() ), _dist ( 0 ), _radius ( 100 ), _fov ( 40 )
                {
                        Eigen::Vector3d b ( 1, 1, 1 );
                        b.normalize();
                        b *= 100;
                        this->init ( -b, b );
                        return;
                }
                Camera ( const Camera& that ) = default;
                Camera ( Camera&& that ) = default;
                Camera& operator = ( const Camera& that ) = default;
                Camera& operator = ( Camera&& that ) = default;
                ~Camera ( void ) = default;

                void init ( const Eigen::Vector3d& bmin, const Eigen::Vector3d& bmax )
                {
                        this->_center = ( bmin + bmax ) * 0.5;
                        this->_radius = ( bmin - bmax ).norm() * 0.5;
                        this->_dist   = this->_radius * 1.0 / std::sin ( this->_fov / 360.0 * 3.1415926 ) ;
                        return;
                }
                void init ( const Eigen::Vector3d& center, const double radius, const double dist )
                {
                        this->_center = center;
                        this->_radius = radius;
                        this->_dist   = dist;
                }

                void clone ( const Camera& that )
                {
                        this->init ( that._center, that._radius, that._dist );
                        this->_rotation = that._rotation;
                        return;
                }

                void getLookAt ( Eigen::Vector3d& eye, Eigen::Vector3d& center, Eigen::Vector3d& up )
                {
                        const auto& rotation = this->_rotation;
                        center = this->_center;
                        const auto& dist   = this->_dist;

                        const auto r = rotation.toRotationMatrix();
                        eye = r * Eigen::Vector3d eye0 ( 0, 0, dist ) + center;
                        up  = r * Eigen::Vector3d up0 ( 0, 1, 0 );
                        return;
                }

                void zoom ( bool isUp  )
                {
			this->_dist *= isUp ? 0.99 : 1.0 / 0.99;
                }

                void getZNearFar ( double& zNear, double& zFar )
                {
                        const double& radius = this->_radius;
                        const double& dist   = this->_dist;

                        zNear = dist - radius;
                        zFar  = dist + radius;
			
			zNear = std::max ( 0.01, zNear);

                }

                double getFov ( void ) const
                {
                        return this->_fov;
                }

                void rotate ( const double  oldx, const double oldy, const double newx, const double newy )
                {
                        auto& rotation = this->_rotation;
                        Eigen::Vector3d oldp ( oldx, oldy, 0.0 );
                        Eigen::Vector3d newp ( newx, newy, 0.0 );

                        if ( oldp.isApprox ( newp, 1.0e-16 ) ) {
                                return;
                        }

                        double radius_virtual_sphere = 0.9; // @todo move to attribute  
                        this->project_onto_sphere ( radius_virtual_sphere, oldp );
                        this->project_onto_sphere ( radius_virtual_sphere, newp );
                        Eigen::Quaterniond dr;
                        dr.setFromTwoVectors ( newp, oldp );
                        rotation *= dr;
                        return;
                }
                /**
                 * @brief Rotate by axis and angle.
                 * @param [in] axis Axis of rotation.
                 * @param [in] angle Rotation angle [rad].
                 */
                void rotate ( const Eigen::Vector3d& axis,  const double angle )
                {
                        Eigen::Quaterniond& rotation = this->_rotation;
                        Eigen::AngleAxisd aa ( angle, axis );
                        Eigen::Quaterniond dr ( aa );
                        rotation *= dr;
                        return;
                }

                /**
                 * @brief Get radius of virtual sphere.
                 */
                double getRadius ( void ) const
                {
                        return this->_radius;
                }

                void moveTo ( const Eigen::Vector3d& newRay )
                {
                        this->_rotation.setIdentity();
                        auto m = this->_rotation.toRotationMatrix();
                        auto oldRay =  m *  Eigen::Vector3d ( 0, 0, 1 );
                        this->_rotation.setFromTwoVectors ( newRay, oldRay );
                }
        private:
                void
                project_onto_sphere ( const double& radius, Eigen::Vector3d& p )
                {
                        p.z() = 0; // project onto xy-plane.
                        const double d = p.x() * p.x() + p.y() * p.y();
                        const double r = radius * radius;

                        if ( d < r )	{
                                p.z() = std::sqrt ( r - d );        // on sphere
                        } else {
                                p *= radius / p.norm();        // on silhouette
                        }

                        return;
                }
        private:
                Eigen::Quaterniond _rotation;	///< rotation
                Eigen::Vector3d _center;	///< center point
                double	 _dist;			///< distance between eye-center.
                double   _radius;		///< radius of bounding sphere.
                double   _fov;			///< field-of-view angle
        };
}
#endif
