/**
 * @file OffScreenRenderer.hpp
 * @brief Class declaration of mi::OffScreenRenderer
 * @author Takashi Michikawa <michikawa@acm.org>
 */
#ifndef MI4_OFFSCREEN_RENDERER_HPP
#define MI4_OFFSCREEN_RENDERER_HPP 1
#include <vector>
#include <memory>
#include <iostream>
#include <GLFW/glfw3.h>

namespace mi4
{
        class OffScreenRenderer
        {
        private:
                OffScreenRenderer&  operator =  ( const OffScreenRenderer& ) = delete;
                OffScreenRenderer&  operator =  ( OffScreenRenderer&& ) = delete;
                OffScreenRenderer ( const OffScreenRenderer& ) = delete;
                OffScreenRenderer ( OffScreenRenderer&& ) = delete;
        private:
                const int _width;
                const int _height;
                bool _isOk;
                GLFWwindow* _window;
        public:
                explicit OffScreenRenderer ( const int width = 128, const int height = 128 ) : _width ( width ), _height ( height ), _isOk ( true ), _window ( NULL )
                {
                        auto* window = this->window();

                        if ( ::glfwInit() == GL_FALSE ) {
                                this->_isOk = false;
                        }

                        ::glfwWindowHint ( GLFW_VISIBLE, 0 );
                        this->_window = ::glfwCreateWindow ( width, height, "tmp", NULL, NULL );
                        ::glfwMakeContextCurrent ( window );

                        if ( !window ) {
                                this->_isOk = false;
                        }

                        if ( !this->_isOk ) {
                                std::cerr << "Init failed." << std::endl;
                        }

                        return;
                }

                ~OffScreenRenderer ( void ) = default;

                int width ( void ) const
                {
                        return this->_width;
                }

                int height ( void ) const
                {
                        return this->_height;
                }

                GLFWwindow* window ( void )
                {
                        return this->_window;
                }
                void swapBuffers ( void )
                {
                        ::glfwSwapBuffers ( this->_window );
                        return;
                }
                /*
                                void getImage ( std::vector<unsigned char>& image )
                                {
                                        const auto width  = this->width();
                                        const auto height = this->height();
                                        const auto npixels = width * height;

                                        image.assign ( npixels * 3 , 0x00 );
                                        unsigned char *buf = new unsigned char [ npixels * 4 ];

                                        ::glReadPixels ( 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, buf );

                                        for ( auto y = 0 ; y < height ; ++y ) {
                                                for ( auto x = 0 ; x < width ; ++x ) {
                                                        const auto idx = y * width + x ;
                                                        const auto idxBuf = 4 * idx;
                                                        const auto idxImg = 3 * idx;
                                                        for ( auto i = 0 ; i < 3 ; ++i ) {
                                                                image[idxImg + i] = buf[idxBuf + i];
                                                        }
                                                }
                                        }
                                        delete[] buf;
                                        return;
                                }

                                void getDepth ( std::vector<float>& depth )
                                {
                                        const auto width  = this->width();
                                        const auto height = this->height();
                                        const auto npixels = width * height;

                                        depth.assign( npixels , 0 );
                                        float *buf = new float [ npixels ];
                                        ::glReadPixels ( 0, 0, width, height, GL_DEPTH_COMPONENT, GL_FLOAT, buf );
                                        for (auto i = 0 ; i < npixels ; ++i ) {
                                                depth[i] = buf[i];
                                        }
                                        delete[] buf;
                                        return;
                                }
                */
        };
}
#endif
