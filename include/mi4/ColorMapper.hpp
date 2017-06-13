#ifndef MI4_COLOR_MAPPER_HPP
#define MI4_COLOR_MAPPER_HPP 1
#include <deque>
#include <tuple>
#include <algorithm>

namespace mi4
{
        class ColorMapper
        {
        private:
                typedef std::tuple<float, float, float, float> value_type;
                std::deque<value_type> _values;
        public:
                void init ( const float minValue = 0.0f, const float maxValue = 1.0f )
                {
                        this->clear();
                        this->add ( minValue,                            0.0f, 0.0f, 1.0f );
                        this->add ( 0.75f * minValue + 0.25f * maxValue, 0.0f, 1.0f, 1.0f );
                        this->add ( 0.5f  * minValue + 0.5f  * maxValue, 0.0f, 1.0f, 0.0f );
                        this->add ( 0.25f * minValue + 0.75f * maxValue, 1.0f, 1.0f, 0.0f );
                        this->add ( maxValue,                            1.0f, 0.0f, 0.0f );
                        this->sort();
                        return;
                }

                void clear ( void )
                {
                        this->_values.clear();
                        return;
                }

                void sort ( void )
                {
                        auto comp_fn = [] ( const value_type & a, const value_type & b ) {
                                return std::get<0> ( a ) < std::get<0> ( b );
                        };
                        std::sort ( this->_values.begin(), this->_values.end(), comp_fn );
                        return;
                }
                void add ( const float value, const float r, const float g, const float b )
                {
                        this->_values.push_back ( std::make_tuple ( value, r, g, b ) );
                        return;
                }

                void convert ( const float value, float& r, float& g, float& b ) const
                {
                        if ( this->_values.size() == 0 ) {
                                r = 0;
                                g = 0;
                                b = 0;
                                return;
                        }

                        auto comp_fn = [] ( const value_type & a, const value_type & b ) {
                                return std::get<0> ( a ) < std::get<0> ( b );
                        };

                        const auto& values = this->_values;
                        const value_type v = std::make_tuple ( value, 0.0f, 0.0f, 0.0f );
                        const auto upper = std::upper_bound ( values.cbegin(), values.cend(), v, comp_fn );

                        if ( upper == values.begin() ) {
                                r = std::get<1> ( *upper );
                                g = std::get<2> ( *upper );
                                b = std::get<3> ( *upper );
                                return;
                        } else if ( upper == values.end() ) {
                                r = std::get<1> ( values.back() );
                                g = std::get<2> ( values.back() );
                                b = std::get<3> ( values.back() );
                                return;
                        } else {
                                const auto lower = upper - 1 ;
                                const float t = ( value - std::get<0> ( *lower ) ) / ( std::get<0> ( *upper ) - std::get<0> ( *lower ) );
                                r = ( 1.0f - t ) * std::get<1> ( *lower ) + t * std::get<1> ( *upper );
                                g = ( 1.0f - t ) * std::get<2> ( *lower ) + t * std::get<2> ( *upper );
                                b = ( 1.0f - t ) * std::get<3> ( *lower ) + t * std::get<3> ( *upper );
                                return;
                        }
                }
                void convert ( const float value, int& r,  int& g, int& b )
                {
                        float r0, g0, b0;
                        this->convert ( value, r0, g0, b0 );
                        r = static_cast<int> ( r0 * 255 ) ;
                        g = static_cast<int> ( g0 * 255 ) ;
                        b = static_cast<int> ( b0 * 255 ) ;
                        return;
                }

                void convert ( const float value, unsigned char& r,  unsigned char& g, unsigned char& b )
                {
                        int r0, g0, b0;
                        this->convert ( value, r0, g0, b0 );
                        r = static_cast<unsigned char> ( r0 ) ;
                        g = static_cast<unsigned char> ( g0 ) ;
                        b = static_cast<unsigned char> ( b0 ) ;
                        return;
                }
        };
}
#endif
