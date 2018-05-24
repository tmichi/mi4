#include <mi4/ColorMapper.hpp>
#include <iostream>
int main (void)
{
        mi4::ColorMapper mapper;
        mapper.init ( 0.1f, 0.9f );
        const int N = 100;

        for ( int i = 0 ; i <= N ; ++i ) {
                const auto t =  i * 1.0f / N ;
                int r, g, b;
                mapper.convert ( t, r, g, b );
                std::cerr << t << " :" << r << "," << g << "," << b << std::endl;
        }

        return 0;
}
