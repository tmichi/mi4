#ifndef MI4_SYSTEM_INFO_HPP
#define MI4_SYSTEM_INFO_HPP 1

#include <iostream>
#include <iomanip>
#include <sstream>
#include <array>
#include <vector>
#include <string>
#include <thread>
#include <chrono>
#include <cstdlib>

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__)
#include <winsock2.h>
#include <iphlpapi.h>
#pragma comment(lib, "IPHLPAPI.lib")

#include <windows.h>
#include <process.h>
#include <intrin.h>
#include <Psapi.h>
#pragma comment(lib, "psapi.lib")

#elif defined (__CYGWIN__)
#include <unistd.h>
#elif defined (__APPLE__)
#include <sys/types.h>
#include <sys/sysctl.h>
#include <sys/resource.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <net/if_dl.h>
#include <net/if.h>
#include <net/if_types.h>
#include <ifaddrs.h>

#elif defined(__linux__)
#include <sys/resource.h>
#include <sys/types.h>

#include <unistd.h>
#include <cstring>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <net/if.h>
#else

#endif

namespace mi4
{
        class MacAddress
        {
        private:
                std::string _ifName;
                std::array<unsigned char, 6> _address;
        public:
                MacAddress ( const std::string& ifName, unsigned char* addr ) : _ifName ( ifName )
                {
                        for ( int i = 0 ; i < 6 ; ++i ) {
                                this->_address[i] = addr[i];
                        }
                        return;
                }

                MacAddress ( const MacAddress& that ) = default;
                MacAddress& operator = ( const MacAddress& that ) = default;
                MacAddress (  MacAddress&& that ) = default;
                MacAddress& operator = ( MacAddress&& that ) = default;
                ~MacAddress ( void ) = default;

                std::string getName ( void ) const
                {
                        return this->_ifName;
                }

                std::string toString ( void ) const
                {
                        std::stringstream ss;
                        ss << std::hex << std::setiosflags ( std::ios::uppercase );
                        for ( int i = 0 ; i < 6 ; ++i ) {
                                ss << std::setw ( 2 ) << std::setfill ( '0' ) << static_cast<int> ( this->_address[i] );

                                if ( i != 5 ) {
                                        ss << ":";
                                }
                        }

                        return ss.str();
                }
        };

#if defined (WIN32) || defined(_WIN32) || defined(__WIN32__)
        class SystemInfoWindows
        {
        public :
                static std::string getCpuName ( void )
                {
                        //ref: https://msdn.microsoft.com/ja-jp/library/hskdteyh.aspx
                        char CPUBrandString[0x40];
                        int CPUInfo[4] = { -1};
                        __cpuid ( CPUInfo, 0x80000000 );
                        unsigned int nExIds = CPUInfo[0];

                        if ( nExIds >= 0x80000004 ) {
                                __cpuid ( CPUInfo, 0x80000002 );
                                std::memcpy ( CPUBrandString, CPUInfo, sizeof ( CPUInfo ) );
                                __cpuid ( CPUInfo, 0x80000003 );
                                std::memcpy ( CPUBrandString + 16, CPUInfo, sizeof ( CPUInfo ) );
                                __cpuid ( CPUInfo, 0x80000004 );
                                std::memcpy ( CPUBrandString + 32, CPUInfo, sizeof ( CPUInfo ) );
                        }

                        return std::string ( CPUBrandString );
                }
                static double getMemorySize ( void )
                {
                        //ref : https://msdn.microsoft.com/ja-jp/library/windows/desktop/aa366589(v=vs.85).aspx
                        MEMORYSTATUSEX stat;
                        stat.dwLength = sizeof ( MEMORYSTATUSEX );

                        if ( GlobalMemoryStatusEx ( &stat ) == 0 ) {
                                return 0;
                        } else {
                                return static_cast<double> ( stat.ullTotalPhys / 1024.0 / 1024 / 1024 ) ;
                        }
                }
                static double getPeakMemorySize ( void )
                {
                        //ref : https://msdn.microsoft.com/ja-jp/library/windows/desktop/ms682050(v=vs.85).aspx
                        double peakMemory = 0;
                        PROCESS_MEMORY_COUNTERS pmc;
                        HANDLE hProcess = OpenProcess ( PROCESS_QUERY_INFORMATION, FALSE, GetCurrentProcessId() );

                        if ( GetProcessMemoryInfo ( hProcess, &pmc, sizeof ( pmc ) ) ) {
                                peakMemory = static_cast<double> ( pmc.PeakWorkingSetSize );
                        }

                        CloseHandle ( hProcess );
                        return peakMemory; //bytes
                }
                static std::vector<MacAddress> getMacAddresses ( void )
                {
                        std::vector<MacAddress> addresses;
                        ULONG ulOutBufLen = sizeof ( IP_ADAPTER_INFO );
                        PIP_ADAPTER_INFO pAdapterInfo = ( IP_ADAPTER_INFO* ) malloc ( ulOutBufLen );

                        if ( pAdapterInfo == NULL ) {
                                return addresses;
                        }

                        if ( GetAdaptersInfo ( pAdapterInfo, &ulOutBufLen ) == ERROR_BUFFER_OVERFLOW ) {
                                free ( pAdapterInfo );
                                pAdapterInfo = ( IP_ADAPTER_INFO* )malloc ( ulOutBufLen );

                                if ( pAdapterInfo == NULL ) {
                                        return addresses;
                                }
                        }

                        if ( GetAdaptersInfo ( pAdapterInfo, &ulOutBufLen ) == NO_ERROR ) {
                                PIP_ADAPTER_INFO pAdapter = pAdapterInfo;

                                while ( pAdapter ) {
                                        switch ( pAdapter->Type ) {
                                        case MIB_IF_TYPE_ETHERNET:
                                                unsigned char* ad = pAdapter->Address;
                                                addresses.push_back ( MacAddress ( std::to_string ( pAdapter->ComboIndex ), ad ) );
                                                break;
                                        }

                                        pAdapter = pAdapter->Next;
                                }
                        }

                        if ( pAdapterInfo != NULL ) {
                                free ( pAdapterInfo );
                        }

                        return addresses;
                }
        };
        using SystemInfoOsSpecific = SystemInfoWindows;
#elif defined (__CYGWIN__)
/// @todo no implementation
        class SystemInfoCygwin
        {
        public :
                static std::string getCpuName ( void )
                {
                        return std::string();
                }
                static double getMemorySize ( void )
                {
                        unsigned long maxpages = sysconf ( _SC_PHYS_PAGES );
                        return maxpages * 4096 * 1.0 / 1024 / 1024 / 1024 ;
                }
                static double getPeakMemorySize ( void )
                {
                        return 0;
                }
                static std::vector<MacAddress> getMacAddresses ( void )
                {
		  std::vector<MacAddress> addresses;
		  return addresses;
		}

        };
        using SystemInfoOsSpecific = SystemInfoCygwin;
#elif defined (__APPLE__)
        class SystemInfoApple
        {
        public :
                static std::string getCpuName ( void )
                {
                        char result[256];
                        size_t size = 256;
                        sysctlbyname ( "machdep.cpu.brand_string", &result[0], &size, NULL, 0 );
                        return std::string ( result );
                }
                static double getMemorySize ( void )
                {
                        uint64_t result;
                        size_t size = sizeof ( result );
                        sysctlbyname ( "hw.memsize", &result, &size, NULL, 0 );
                        return static_cast<double> ( result ) * 1.0 / 1024.0 / 1024.0 / 1024.0; //GB
                }
                ///@todo The result somewhat strange on Mac.
                static double getPeakMemorySize ( void )
                {
                        double peakMemory = 0;
                        struct rusage rusage;
                        getrusage ( RUSAGE_SELF, &rusage );
                        peakMemory = static_cast<double> ( rusage.ru_maxrss );
                        return peakMemory; //
                }
                static std::vector<MacAddress> getMacAddresses ( void )
                {
                        std::vector<MacAddress> addresses;
                        ifaddrs* iflist;

                        if ( getifaddrs ( &iflist ) == 0 ) {
                                for ( ifaddrs* iter = iflist ;  iter ; iter = iter->ifa_next ) {
                                        sockaddr_dl* dl = reinterpret_cast<sockaddr_dl*> ( iter->ifa_addr );

                                        if ( dl->sdl_family == AF_LINK && dl->sdl_type == IFT_ETHER ) {
                                                unsigned char* addr = reinterpret_cast<unsigned char*> ( LLADDR ( dl ) );
                                                addresses.push_back ( MacAddress ( iter->ifa_name, addr ) );
                                        }
                                }
                        }

                        freeifaddrs ( iflist );
                        return addresses;
                }
        };
        using SystemInfoOsSpecific = SystemInfoApple;
#elif defined (__linux__)
        class SystemInfoLinux
        {
        public:
                static std::string getCpuName ( void )
                {
                        FILE* cmd = popen ( "grep 'model name' /proc/cpuinfo", "r" );

                        if ( cmd == NULL ) {
                                return std::string ( "error" );
                        }

                        char buf[256];

                        if ( fread ( buf, 1, sizeof ( buf ) - 1, cmd ) <= 0 ) {
                                return std::string ( "Unknown CPU" );
                        }

                        pclose ( cmd );
                        std::string buffer ( buf );
                        auto s = buffer.find_first_of ( ":" ) + 2 ;
                        return buffer.substr ( s, buffer.find ( "\n" ) - s );
                }


                static double getMemorySize ( void )
                {
                        FILE* cmd = popen ( "grep 'MemTotal' /proc/meminfo", "r" );

                        if ( cmd == NULL ) {
                                return 0;
                        }

                        char buff[256];

                        if ( fread ( buff, 1, sizeof ( buff ) - 1, cmd ) <= 0 ) {
                                return 0;
                        }

                        pclose ( cmd );
                        std::string buffer ( buff );
                        return std::stod ( buffer.substr ( buffer.find_first_of ( ":" ) + 1, buffer.find ( "kB" ) ) ) / 1024 / 1024;
                }
                static double getPeakMemorySize ( void )
                {
                        struct rusage rusage;
                        getrusage ( RUSAGE_SELF, &rusage );
                        return static_cast<double> ( rusage.ru_maxrss ) * 1024L;
                }
                static std::vector<MacAddress> getMacAddresses ( void )
                {
                        std::vector<MacAddress> addresses;
                        int fd = socket ( AF_INET, SOCK_DGRAM, 0 );
                        ifconf ifc;
                        ifreq ifs[16];
                        ifc.ifc_len = sizeof ( ifreq ) * 16;
                        ifc.ifc_req = ifs;

                        if ( ioctl ( fd, SIOCGIFCONF, &ifc ) == 0 ) {
                                ifreq* fend = ifs + ( ifc.ifc_len / sizeof ( ifreq ) );

                                for ( ifreq* ifr = ifc.ifc_req; ifr < fend ; ifr++ ) {
                                        if ( ifr->ifr_addr.sa_family == AF_INET ) {
                                                ifreq ifreq0;
                                                std::strncpy ( ifreq0.ifr_name, ifr->ifr_name, sizeof ( ifreq0.ifr_name ) );

                                                if ( ioctl ( fd, SIOCGIFHWADDR, &ifreq0 ) < 0 ) {
                                                        continue;
                                                }

                                                unsigned char* address = reinterpret_cast<unsigned char*> ( ifreq0.ifr_hwaddr.sa_data );
                                                addresses.push_back ( MacAddress ( ifreq0.ifr_name, address ) );
                                        }
                                }
                        }

                        close ( fd );
                        return addresses;
                }
        };
        using SystemInfoOsSpecific = SystemInfoLinux;
#else
        /// @todo no implementation
        class SystemInfoUnknown
        {
        public :
                static std::string getCpuName ( void )
                {
                        return std::string ( "good cpu" );
                }
                static double getMemorySize ( void )
                {
                        return 0;
                }
                static double getPeakMemorySize ( void )
                {
                        return 0;
                }
                static std::vector<MacAddress> getMacAddresses ( void )
                {
                        std::vector<MacAddress> addresses;
                        return addresses;
                }
        };
        using SystemInfoOsSpecific = SystemInfoUnknown;
#endif
        class SystemInfo
        {
        public:
                /**
                 * @brief return CPU name
                 * @return CPU name
                 */
                static std::string getCpuName ( void )
                {
                        return SystemInfoOsSpecific::getCpuName();
                }

                /**
                 * @brief return memory size in giga bytes
                 * @return memory size in GB
                 */
                static double getMemorySize ( void )
                {
                        return SystemInfoOsSpecific::getMemorySize();
                }

                /**
                 * @brief return the number of cores
                 * @return the number of cores
                 */
                static int getNumCores ( void )
                {
                        return static_cast<int> ( std::thread::hardware_concurrency() );
                }

                /**
                 * @brief return date as string.
                 * @return date
                 */
                static std::string getDate ( void )
                {
                        std::time_t t = std::time ( nullptr );
                        std::string str ( std::asctime ( std::localtime ( &t ) ) );
                        return str.substr ( 0, str.find_last_of ( "\n" ) );
                }
                /**
                 * @brief return the peak memory size
                 * @param [in] unit unit (default : bytes)
                 * @return peak memory size
                 */
                static double getPeakMemorySize ( void )
                {
                        return SystemInfoOsSpecific::getPeakMemorySize();
                }
	  
	        /**
		 * @brief Get MAC addresses. 
		 * @return List of Mac addresses
		 */
	        static std::vector<MacAddress> getMacAddresses ( void )
	        {
                        return SystemInfoOsSpecific::getMacAddresses();
                }
        };
}
#endif
