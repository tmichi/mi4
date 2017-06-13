#include <mi4/SystemInfo.hpp>
#include <vector>


int main ( int argc, char** argv )
{
        std::vector<mi4::MacAddress> addresses = mi4::SystemInfo::getMacAddresses();

        for ( auto i : addresses ) {
                std::cerr << i.getName() << "\t" << i.toString() << std::endl;
        }

        return 0;
}
