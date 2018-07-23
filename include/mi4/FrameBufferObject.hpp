#ifndef MI4_FRAMEBUFFEROBJECT_HPP
#define MI4_FRAMEBUFFEROBJECT_HPP 1
#include <vector>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
namespace mi4
{
        class FrameBufferObject
        {
        private:
                const int width_;
                const int height_;
                GLuint fbo_; // frame buffer object
                GLuint cbo_; //   +-- color buffer objcet
                GLuint rbo_; //   +-- render buffer object
        private:
                FrameBufferObject ( const FrameBufferObject& that ) = delete;
                FrameBufferObject ( FrameBufferObject&& that ) = delete;
                void operator = ( const FrameBufferObject& that ) = delete;
                void operator = ( FrameBufferObject&& that ) = delete;
        public:
                explicit FrameBufferObject (const int width, const int height) : width_(width), height_(height)
                {
                        auto& fbo = this->fbo_;
                        auto& cbo = this->cbo_;
                        auto& rbo = this->rbo_;

                        ::glGenTextures(1, &cbo);

                        ::glBindTexture(GL_TEXTURE_2D, cbo);
                        ::glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_FLOAT, 0);
                        ::glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
                        ::glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
                        ::glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
                        ::glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
                        ::glBindTexture(GL_TEXTURE_2D, 0);

                        ::glGenRenderbuffersEXT ( 1, &rbo );
                        ::glBindRenderbufferEXT ( GL_RENDERBUFFER_EXT, rbo );
                        ::glRenderbufferStorageEXT ( GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT, width, height );
                        ::glBindRenderbufferEXT ( GL_RENDERBUFFER_EXT, 0 );

                        ::glGenFramebuffersEXT ( 1, &fbo );
                        ::glBindFramebufferEXT ( GL_FRAMEBUFFER_EXT, fbo );
                        ::glFramebufferTexture2DEXT ( GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, cbo, 0 );
                        ::glFramebufferRenderbufferEXT ( GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, rbo );
                        ::glBindFramebufferEXT ( GL_FRAMEBUFFER_EXT, 0 );

                        return;
                };

                ~FrameBufferObject ( void )
                {
                        ::glDeleteFramebuffersEXT(1, &(this->fbo_));
                        ::glDeleteRenderbuffersEXT(1, &(this->rbo_));
                        ::glDeleteTextures(1, &(this->cbo_));
                        return;
                }

                void activate ( void )
                {
                        ::glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, this->fbo_);
                        return;
                }

                void deactivate ( void )
                {
                        ::glBindFramebufferEXT ( GL_FRAMEBUFFER_EXT, 0 );
                        return;
                }

                inline int getWidth ( void ) const
                {
                        return this->width_;
                }

                inline int getHeight ( void ) const
                {
                        return this->height_;
                }

                void getBuffer ( std::vector<GLfloat>& buffer )
                {
                        buffer.assign ( this->getWidth() * this->getHeight() * 4, 0 ) ;
                        ::glBindTexture(GL_TEXTURE_2D, this->cbo_);
                        ::glGetTexImage ( GL_TEXTURE_2D, 0, GL_RGBA, GL_FLOAT, & ( buffer[0] ) );
                        ::glBindTexture ( GL_TEXTURE_2D, 0 );
                        return;
                }
        };
}
#endif//
