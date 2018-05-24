#include <vector>
#include <iostream>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <mi4/FrameBufferObject.hpp>

// Use https://github.com/nothings/stb/blob/master/stb_image_write.h
#define STB_IMAGE_WRITE_IMPLEMENTATION 1
#include "stb/stb_image_write.h"

int main ( int argc, char** argv )
{
        if ( ::glfwInit() == GL_FALSE )  {
                std::cerr << "init failed" << std::endl;
                return -1;
        }

        // Offscreen
        ::glfwWindowHint ( GLFW_VISIBLE, 0 );
        const int width  = 64;
        const int height = 64;

        GLFWwindow* window = ::glfwCreateWindow ( width, height, "tmp", NULL, NULL );
        ::glfwMakeContextCurrent ( window );
        GLenum err = glewInit();

        if ( err != GLEW_OK ) {
                fprintf ( stderr, "Error: %s\n", glewGetErrorString ( err ) );
                return -1;
        }

        if ( 1 ) {
                const int w = 129;
                const int h = 65;
                mi4::FrameBufferObject fbo ( w, h );

                fbo.activate();
                glClearColor ( 0.0f, 0.0f, 1.0f, 1.0f );
                glEnable ( GL_DEPTH_TEST );
                glMatrixMode ( GL_PROJECTION );
                glViewport ( 0, 0, w, h );
                glLoadIdentity();
                glOrtho ( 0, w, 0, h, -1, 1 );
                glMatrixMode ( GL_MODELVIEW );
                glLoadIdentity();
                glClear ( GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT );

                glBegin ( GL_POLYGON );
                glColor3f ( 1, 0, 0 );
                glVertex2f ( 10, 10 );
                glColor3f ( 0, 1, 0 );
                glVertex2f ( 110, 10 );
                glColor3f ( 0, 0, 1 );
                glVertex2f ( 110, 60 );
                glColor3f ( 0, 0, 0 );
                glVertex2f ( 10, 60 );
                glEnd();

                glFlush();

                std::vector<unsigned char> pixel;
                std::vector<float> buffer;
                fbo.getBuffer ( buffer );

                for ( auto& b : buffer ) {
                        pixel.push_back ( static_cast<unsigned char> ( b * 255 ) );
                }

                fbo.deactivate();

                stbi_write_png ( "fbo.png", w, h, 4, &pixel[0], 4 * w );
        }

        glfwTerminate();
        return 0;
}
