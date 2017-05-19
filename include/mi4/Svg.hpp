/**
* @file Svg.hpp
* @author Takashi Michikawa <michikawa@acm.org>
*/
#ifndef MI_SVG_HPP
#define MI_SVG_HPP 1
#include <string>
#include <iostream>
#include <cmath>
#include "Xml.hpp"
namespace mi4
{
        class Svg : public XmlDocument
        {
        private:
                class Vector2d
                {
                public:
                        double x;
                        double y;
                public:
                        explicit Vector2d ( const double x0 = 0, const double y0 = 0 ) : x ( x0 ), y ( y0 )
                        {
                                return;
                        }
                        Vector2d ( const Vector2d& that ) = default;
                        Vector2d& operator = ( const Vector2d& that ) = default;
                        Vector2d ( Vector2d&& that ) = default;
                        Vector2d& operator = ( Vector2d&& that ) = default;
                        ~Vector2d ( void ) = default;
                };
        private:
                int _stroke_dashed;
                double _stroke_width;
                std::string _stroke_color;
                std::string _fill_color;

                Svg::Vector2d _bmin;
                Svg::Vector2d _bmax;
                Svg::Vector2d _size;
        public:
                Svg ( const int width, const int height ) : XmlDocument ( "svg" ), _size ( width, height )
                {
                        this->init();
                        this->setViewBox ( 0, 0, this->_size.x, this->_size.y );

                        XmlElement& root = this->getRoot();
                        root.addAttribute ( "xmlns", "http://www.w3.org/2000/svg" );
                        root.addAttribute ( "xmlns:xlink", "http://www.w3.org/1999/xlink" );
                        root.addAttribute ( "width",  this->_size.x );
                        root.addAttribute ( "height", this->_size.y );
                        return;
                }

                ~Svg ( void ) = default;

                void setViewBox ( const double mnx, const double mny, const double mxx, const double mxy )
                {
                        if ( std::fabs ( mxx - mnx ) < 1.0e-10 || std::fabs ( mxy - mny ) < 1.0e-10 ) {
                                std::cerr << "warning : invalid viewport size" << "(" << mnx << "," << mny << ")" << "-" << "(" << mxx << "," << mxy << ")" << std::endl;
                        }

                        this->_bmin = Svg::Vector2d ( mnx, mny );
                        this->_bmax = Svg::Vector2d ( mxx, mxy );
                        return;
                }


                void init ( void )
                {
                        this->_stroke_dashed = 0;
                        this->setStrokeWidth ( 1.0 );
                        this->setStrokeColor ( std::string ( "#000000" ) );
                        this->setFillColor ( std::string ( "#000000" ) );
                        return;
                }

                void setStrokeWidth ( const double width )
                {
                        this->_stroke_width = width;
                }

                void setStrokeColor ( const std::string& col )
                {
                        this->_stroke_color = col;
                }

                void setFillColor ( const std::string& col )
                {
                        this->_fill_color = col;
                }

                void setStrokeDash ( const int dash_pitch )
                {
                        this->_stroke_dashed = dash_pitch;
                }

                void drawImage ( const std::string& path, const double x0, const double y0,  const double width, const double height )
                {
                        auto size = this->convertTo ( Svg::Vector2d ( x0, y0 ) );
                        auto v0   = this->convertTo ( Svg::Vector2d ( width, height ) );
                        XmlElement& element = this->getRoot().addChildElement ( "image" );
                        element.addAttribute ( "xlink:href", path ) ;
                        element.addAttribute ( "x", v0.x ) ;
                        element.addAttribute ( "y", v0.y ) ;
                        element.addAttribute ( "width", size.x ) ;
                        element.addAttribute ( "height", size.y ) ;
                        return;
                }

                void drawLine ( const double x0, const double y0, const double x1, const double y1 )
                {
                        auto v0 = this->convertTo ( Svg::Vector2d ( x0, y0 ) );
                        auto v1 = this->convertTo ( Svg::Vector2d ( x1, y1 ) );
                        XmlElement& element = this->getRoot().addChildElement ( "line" );
                        element.addAttribute ( "x1", v0.x ) ;
                        element.addAttribute ( "y1", v0.y ) ;
                        element.addAttribute ( "x2", v1.x ) ;
                        element.addAttribute ( "y2", v1.y ) ;

                        if ( this->_stroke_dashed > 0 ) {
                                element.addAttribute ( "stroke-dasharray", this->_stroke_dashed );
                        }

                        element.addAttribute ( "stroke-width", this->_stroke_width );
                        element.addAttribute ( "stroke", this->_stroke_color );
                        return;
                }

                void drawCircle ( const double cx, const double cy, const double r )
                {
                        auto v0 = this->convertTo ( Svg::Vector2d ( cx, cy ) );
                        XmlElement& element = this->getRoot().addChildElement ( "circle" );
                        element.addAttribute ( "cx", v0.x ).addAttribute ( "cy", v0.y ).addAttribute ( "r", r ).addAttribute ( "fill", this->_fill_color );

                        if ( this->_stroke_dashed > 0 ) {
                                element.addAttribute ( "stroke-dasharray", this->_stroke_dashed );
                        }

                        return;
                }

                void drawRect ( const double x0, const double y0, const double w, const double h )
                {
                        auto v0 = this->convertTo ( Svg::Vector2d ( x0, y0 ) );
                        auto v1 = this->convertTo ( Svg::Vector2d ( w + x0, h + y0 ) );

                        const double minx = v0.x < v1.x ? v0.x : v1.x;
                        const double miny = v0.y < v1.y ? v0.y : v1.y;
                        const double maxx = v0.x > v1.x ? v0.x : v1.x;
                        const double maxy = v0.y > v1.y ? v0.y : v1.y;
                        const double sizex = maxx - minx;
                        const double sizey = maxy - miny;

                        XmlElement& element = this->getRoot().addChildElement ( "rect" );
                        element.addAttribute ( "x", minx ) ;
                        element.addAttribute ( "y", miny ) ;
                        element.addAttribute ( "width", sizex ) ;
                        element.addAttribute ( "height", sizey ) ;

                        if ( this->_stroke_dashed > 0 ) {
                                element.addAttribute ( "stroke-dasharray", this->_stroke_dashed );
                        }

                        element.addAttribute ( "stroke-width", std::to_string ( this->_stroke_width ) );
                        element.addAttribute ( "stroke", this->_stroke_color );
                        return;
                }
        private:
                void convert ( Vector2d& p )
                {
                        /*                        const double s = ( p.x - this->_bmin.x ) / ( this->_bmax.x - this->_bmin.x ) ;
                                                const double t = ( p.y - this->_bmin.y ) / ( this->_bmax.y - this->_bmin.y ) ;
                                                p.x = s * this->_size.x ;
                                                p.y = ( 1.0 - t ) * this->_size.y ;
                        */

                        return;
                }

                Svg::Vector2d convertTo ( const Vector2d& v ) const
                {
                        return v;
                }
        };
}
#endif// MI_SVG_HPP
