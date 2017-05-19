#ifndef MI_OPEN_GL_UTILITY_HPP
#define MI_OPEN_GL_UTILITY_HPP 1
#include <Eigen/Dense>
#include <vector>
#if defined(__APPLE__)
#include <OpenGL/gl.h>
#else
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__)
#include <windows.h>
#pragma comment(lib, "opengl32.lib")
#endif
#include <GL/gl.h>
#endif

namespace mi4
{
        class OpenGlUtility
        {
        public:
                static void lookAt ( const  Eigen::Vector3d& eye, const Eigen::Vector3d& center, const Eigen::Vector3d& up )
                {
                        const auto forward = ( center - eye ).normalized();
                        const auto side = forward.cross ( up ).normalized();
                        const auto upv  = side.cross ( forward ).normalized();
                        std::vector<double> m ( 16, 0 );

                        for ( int i = 0  ; i < 3 ; ++i ) {
                                m[i * 4 + 0] = side[i];
                                m[i * 4 + 1] = upv[i];
                                m[i * 4 + 2] = -forward[i];
                        }

                        m[3 * 4 + 3] = 1.0;
                        ::glMultMatrixd ( m.data() );
                        ::glTranslated ( -eye.x(), -eye.y(), -eye.z() );
                        return;
                }
                static void perspective ( const double fov, const double aspect, const double znear, const double zfar )
                {
                        std::vector<double> m ( 16, 0 );
                        double f = 1.0 / std::tan ( 0.5 * fov * M_PI / 180.0 );
                        m[0] = f * 1.0 / aspect;
                        m[1 * 4 + 1] = f;

                        m[2 * 4 + 2] = ( zfar + znear ) * 1.0 / ( znear - zfar ) ;
                        m[2 * 4 + 3] = -1.0;
                        m[3 * 4 + 2] =  2.0 * zfar * znear / ( znear - zfar ) ;
                        ::glMultMatrixd ( m.data() );
                }
                static void ortho ( const double left, const double  right, const double  bottom, const double  top, const double znear, const double zfar )
                {
                        ::glOrtho ( left, right, bottom, top, znear, zfar );
                        return;
                }
                static void ortho2d ( const double  left, const double  right, const double  bottom, const double  top )
                {
                        OpenGlUtility::ortho ( left, right, bottom, top, -1, 1 );
                        return;
                }
        };
}
#endif
