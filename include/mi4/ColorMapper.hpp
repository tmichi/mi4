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
                using value_type = std::tuple< float, float, float, float >; // value, r, g, b
                std::deque< value_type > _values;
        public:
                void init ( const float minValue = 0.0f, const float maxValue = 1.0f )
                {
                        this->_values.clear();
                        this->add(minValue, 0.0f, 0.0f, 1.0f);
                        this->add ( 0.75f * minValue + 0.25f * maxValue, 0.0f, 1.0f, 1.0f );
                        this->add(0.50f * minValue + 0.50f * maxValue, 0.0f, 1.0f, 0.0f);
                        this->add ( 0.25f * minValue + 0.75f * maxValue, 1.0f, 1.0f, 0.0f );
                        this->add(maxValue, 1.0f, 0.0f, 0.0f);
                        this->sort();
                        return;
                }

                void add ( const float value, const float r, const float g, const float b )
                {
                        this->_values.push_back ( std::make_tuple ( value, r, g, b ) );
                        this->sort();
                        return;
                }

                std::tuple< float, float, float > get (const float value) const
                {
                        float r = 0, g = 0, b = 0;
                        if ( this->_values.size() > 0 ) {
                                const auto& values = this->_values;
                                const auto v = std::make_tuple(value, 0.0f, 0.0f, 0.0f);
                                const auto upper = std::upper_bound(values.cbegin(), values.cend(), v, [] (const value_type& a, const value_type& b) {
                                        return std::get< 0 >(a) < std::get< 0 >(b);
                                });
                                if ( upper == values.begin()) std::tie(std::ignore, r, g, b) = *upper;
                                else if ( upper == values.end()) std::tie(std::ignore, r, g, b) = values.back();
                                else {
                                        const auto lower = upper - 1;
                                        const float t = (value - std::get< 0 >(*lower)) / (std::get< 0 >(*upper) - std::get< 0 >(*lower));
                                        r = (1.0f - t) * std::get< 1 >(*lower) + t * std::get< 1 >(*upper);
                                        g = (1.0f - t) * std::get< 2 >(*lower) + t * std::get< 2 >(*upper);
                                        b = (1.0f - t) * std::get< 3 >(*lower) + t * std::get< 3 >(*upper);
                                }
                        }
                        return std::make_tuple(r, g, b);
                }

                void convert (const float value, float& r, float& g, float& b) const
                {
                        std::tie(r, g, b) = this->get(value);
                        return;
                }
                void convert (const float value, int& r, int& g, int& b)
                {
                        const auto v = this->get(value);
                        r = static_cast<int> ( std::get< 0 >(v) * 255 );
                        g = static_cast<int> ( std::get< 1 >(v) * 255 );
                        b = static_cast<int> ( std::get< 2 >(v) * 255 );
                        return;
                }

                void convert (const float value, unsigned char& r, unsigned char& g, unsigned char& b)
                {
                        const auto v = this->get(value);
                        r = static_cast<unsigned char> ( std::get< 0 >(v) * 255 );
                        g = static_cast<unsigned char> ( std::get< 1 >(v) * 255 );
                        b = static_cast<unsigned char> ( std::get< 2 >(v) * 255 );
                        return;
                }

        private:
                void sort (void)
                {
                        std::sort(this->_values.begin(), this->_values.end(), [] (const value_type& a, const value_type& b) {
                                return std::get< 0 >(a) < std::get< 0 >(b);
                        });
                        return;
                }

        };
}


#endif






