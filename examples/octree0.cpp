/**
 * @file octree.cpp
 * @brief Octree example code.
 * @author Takashi Michikawa <michikawa@acm.org>
 */
#include <iostream>
#include <fstream>
#include <mi4/Octree.hpp>


int main ( void )
{
        const std::string oct_name ( "out.oct" );
        mi4::Octree<short> oct ( 32, 0 ); //Create 32 x 32 x 32 octree ( 5 levels ).
        oct.set ( 20, 30, 0, 4 );
        std::ofstream fout ( oct_name.c_str(), std::ios::binary );

        if ( !oct.write ( fout ) ) {
                std::cerr << "write failed" << std::endl;
                return -1;
        }

        fout.close();
        mi4::Octree<short> oct2;
        std::ifstream fin ( oct_name.c_str(), std::ios::binary );

        if ( ! oct2.read ( fin ) ) {
                return -2;
        }

        fin.close();
        std::cerr << ( int )oct.get ( 20, 30, 0 ) << " " << ( int )oct2.get ( 20, 30, 0 ) << std::endl;
        return 0;
}
