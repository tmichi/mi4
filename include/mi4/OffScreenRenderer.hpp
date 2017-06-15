/**
 * @file OffScreenRenderer.hpp
 * @brief Class declaration of mi4::OffScreenRenderer
 * @author Takashi Michikawa <michikawa@acm.org>
 */
#ifndef MI4_OFFSCREEN_RENDERER_HPP
#define MI4_OFFSCREEN_RENDERER_HPP 1
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
    GLFWwindow* _window;
public:
    explicit OffScreenRenderer ( const int width = 128, const int height = 128 ) : _width ( width ), _height ( height ), _window ( NULL )
    {
        if ( !::glfwInit() ) {
            std::cerr << "glfwInit() failed." << std::endl;
            return;
        }

        ::glfwWindowHint ( GLFW_VISIBLE, 0 );
        this->_window = ::glfwCreateWindow ( width, height, "tmp", NULL, NULL );

        if ( !this->_window ) {
            std::cerr << "glfwCreateWindow() failed." << std::endl;
            return;
        }

        this->makeCurrent();
        return;
    }

    ~OffScreenRenderer ( void )
    {
        ::glfwTerminate();
        return;
    }

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
        ::glfwSwapBuffers ( this->window() );
        return;
    }

    void makeCurrent ( void )
    {
        ::glfwMakeContextCurrent ( this->window() );
        return;
    }
};
}
#endif
