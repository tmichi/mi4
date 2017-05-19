#ifndef MI4_SYSTEM_INFO_HPP
#define MI4_SYSTEM_INFO_HPP 1
#include <string>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <thread>
#include <chrono>
#include <ctime>
#include <array>
#include <ctime>
#include <memory>

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__)
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
#elif defined(__linux__)
#include <sys/resource.h>
#include <sys/types.h>

#else

#endif // defined(WIN32) || defined(_WIN32) || defined(__WIN32__)

namespace mi4
{
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
                        }

                        return static_cast<double> ( stat.ullTotalPhys / 1024.0 / 1024 / 1024 ) ;
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
        };
        using SystemInfoOsSpecific = SystemInfoCygwin;
#elif defined (__APPLE__)
        class SystemInfoPosix
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
        };
        using SystemInfoOsSpecific = SystemInfoPosix;
#elif defined (__linux__)
        class SystemInfoLinux
        {
        public:
                static std::string getCpuName ( void )
                {
                        FILE* cmd = popen ( "grep 'model name' /proc/cpuinfo", "r" );
                        if ( cmd == NULL ) return std::string ( "error" );
                        char buf[256];
                        if ( fread ( buf, 1, sizeof ( buf ) - 1, cmd ) <= 0 ) return std::string ( "Unknown CPU" );
                        pclose ( cmd );
                        std::string buffer ( buf );
                        auto s = buffer.find_first_of ( ":" ) + 2 ;
                        return buffer.substr ( s, buffer.find ( "\n" ) - s );
                }


                static double getMemorySize ( void )
                {
                        FILE* cmd = popen ( "grep 'MemTotal' /proc/meminfo", "r" );
                        if ( cmd == NULL ) return 0;
                        char buff[256];
                        if ( fread ( buff, 1, sizeof ( buff ) - 1, cmd ) <= 0 ) return 0;
                        pclose ( cmd );
                        std::string buffer ( buff );
			// MemTotal:        8069288 kB
                        return std::stod ( buffer.substr ( buffer.find_first_of ( ":" ) + 1, buffer.find ( "kB" ) ) ) / 1024 / 1024;
                }
                static double getPeakMemorySize ( void )
                {
                        struct rusage rusage;
                        getrusage ( RUSAGE_SELF, &rusage );
                        return static_cast<double> ( rusage.ru_maxrss ) * 1024L;
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
                 * @return memory size
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
        };
}
#endif
