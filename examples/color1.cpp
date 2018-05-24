#include <mi4/ColorMapper.hpp>
#include <iostream>
int main ( int argc, char** argv )
{
        mi4::ColorMapper mapper;
        mapper.init ( 0.1f, 0.9f );
        const int N = 1000;

        for ( int i = 0 ; i <= N ; ++i ) {
                const auto t =  i * 1.0f / N ;
                int r, g, b;
                std::tie ( r, g, b ) = mapper.get ( t );
                std::cerr << t << " :" << r << "," << g << "," << b << std::endl;
        }

        return 0;
}
