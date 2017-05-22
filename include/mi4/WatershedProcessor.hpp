#ifndef MI4_WATERSHED_PROCESSOR_HPP
#define MI4_WATERSHED_PROCESSOR_HPP 1
#include "VolumeData.hpp"
#include "SystemInfo.hpp"
#include "PriorityQueue.hpp"
namespace mi4
{
        template <typename T>
        class WatershedProcessor
        {
        private:
                const VolumeData<float>& _weightData;
        public:
                WatershedProcessor ( const VolumeData<float>& weightData ) :  _weightData( weightData ) {
                        return;
                }

                bool process ( VolumeData<T>& labelData ) {
                        const auto& info = const_cast<VolumeData<T>&>( labelData ).getInfo();
                        if ( 1 ) { //@ the program will be failed when "if" is removed.
                                PriorityQueue <Point3i> pq;

                                for( const auto& p : Range( info.getMin(), info.getMax()) ) {
                                        if ( labelData.get( p ) > 0 && this->_weightData.get( p ) > 0 ) {
                                                pq.push( p, -this->_weightData.get( p ) ) ;
                                        }
                                }

                                while( !pq.empty() ) {
                                        const auto p = pq.getTopIndex();
                                        pq.pop();
                                        const auto labelId = labelData.get( p ); // Label ID to be propagated.
                                        for( const auto& d: mi4::Range(mi4::Point3i(-1, -1, -1), mi4::Point3i(1, 1, 1))) {
                                                if ( std::abs(d.x()) + std::abs(d.y())+ std::abs(d.z()) != 1 ) continue;
                                                const Point3i np = p + d;
                                                if ( !info.isValid( np ) ) continue;
                                                if ( labelData.get( np ) != 0 ) continue;
                                                if ( this->_weightData.get( np ) > 0 ) {
                                                        labelData.set( np, labelId );
                                                        pq.push( np, -this->_weightData.get( np ) );
                                                }
                                        }
                                }
                        }
                        return true;
                }
        };
}
#endif// MI4_WATERSHED_PROCESSOR_HPP
