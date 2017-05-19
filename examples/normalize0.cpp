#include "mi4/Normalizer.hpp"
#include <iostream>

int main ( int argc, char** argv )
{
        Eigen::Vector3d bmin ( 1, 1, 1 ) ;
        Eigen::Vector3d bmax ( 3, 3, 3 ) ;

        Eigen::Vector3d lmin ( 0.5, 0.5, 0.5 ) ;
        Eigen::Vector3d lmax ( 1, 1, 1 ) ;

        mi4::Normalizer normalizer ( bmin, bmax, lmin, lmax );

        std::cerr << normalizer.normalize ( Eigen::Vector3d ( 2, 2, 2 ) ).transpose() << std::endl;
        std::cerr << normalizer.normalize ( Eigen::Vector3d ( 1, 2, 3 ) ).transpose() << std::endl;
        std::cerr << normalizer.denormalize ( normalizer.normalize ( Eigen::Vector3d ( 2, 2, 2 ) ) ).transpose() << std::endl;

        return 0;
}
