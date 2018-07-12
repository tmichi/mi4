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
        private:
                using map_mat = Eigen::Map< Eigen::Matrix4d >;
                Projector (const Projector& that) = delete;
                Projector (Projector&& that) = delete;
                void operator = (const Projector& that) = delete;
                void operator = (Projector&& that) = delete;
        public:
                explicit Projector (double modelview[16], double projection[16], int vp[4]) : _matrix(map_mat(projection) * map_mat(modelview)), _inv_matrix((map_mat(projection) * map_mat(modelview)).inverse()), _viewport({vp[0], vp[1], vp[2], vp[3]}) {}
                explicit Projector (const Eigen::Matrix4d& modelview, const Eigen::Matrix4d& projection, const std::array< int, 4 >& vp) : _matrix(projection * modelview), _inv_matrix((projection * modelview).inverse()), _viewport(vp) {}
                ~Projector ( void ) = default;

                Eigen::Vector2d project ( const Eigen::Vector3d& p, double* depth )
                {
                        const auto& vp = this->_viewport;
                        const auto p0 = this->_matrix * Eigen::Vector4d(p.x(), p.y(), p.z(), 1);

                        if ( p0.w() == 0 ) {
                                return Eigen::Vector2d();
                        }

                        *depth = (1.0 + p0.z() / p0.w()) * 0.5;
                        return Eigen::Vector2d(vp[0] + (1 + p0.x() / p0.w()) * vp[2] * 0.5, vp[1] + (1 + p0.y() / p0.w()) * vp[3] * 0.5);
                }

                Eigen::Vector3d unproject (const Eigen::Vector2d& wp, double depth) const
                {
                        const auto& inv_matrix = this->_inv_matrix;
                        const auto& vp = this->_viewport;
                        const Eigen::Vector4d p1 = inv_matrix * Eigen::Vector4d((wp.x() - vp[0]) * 2 / vp[2] - 1.0, (wp.y() - vp[1]) * 2 / vp[3] - 1.0, 2.0 * depth - 1.0, 1.0);
                        return (p1.w() != 0.0) ? 1.0 / p1.w() * Eigen::Vector3d(p1.x(), p1.y(), p1.z()) : Eigen::Vector3d();
                }
        private:
                const Eigen::Matrix4d _matrix;
                const Eigen::Matrix4d _inv_matrix;
                const std::array< int, 4 > _viewport;
        };
}

#endif// MI_PROJECTOR_HPP
