#include <mi4/SystemInfo.hpp>
#include <iostream>
int main ( int argc, char** argv )
{
        std::cerr << mi4::SystemInfo::getCpuName() << std::endl;
        std::cerr << mi4::SystemInfo::getMemorySize() << "GB" << std::endl;
        std::cerr << mi4::SystemInfo::getNumCores() << "Core" << std::endl;
        std::cerr << mi4::SystemInfo::getDate() << std::endl;
        std::cerr << "peak memory size : " << mi4::SystemInfo::getPeakMemorySize() / 1024 / 1024 << "mb" << std::endl;
        return 0;
}
