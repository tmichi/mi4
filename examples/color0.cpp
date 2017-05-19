#include <mi4/ColorMapper.hpp>
#include <iostream>
int main ( int argc, char** argv )
{
        mi4::ColorMapper mapper;
        mapper.init ( 0.1, 0.9 );
        const int N = 1000;

        for ( int i = 0 ; i <= N ; ++i ) {
                const auto t =  i * 1.0 / N ;
                int r, g, b;
                mapper.convert ( t, r, g, b );
                std::cerr << t << " :" << r << "," << g << "," << b << std::endl;
        }

        return 0;
}
