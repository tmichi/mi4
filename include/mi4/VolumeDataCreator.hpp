/**
 * @file VolumeDataCreator.hpp
 * @author Takashi Michikawa
 */
#ifndef MI4_VOLUME_DATA_CREATOR_HPP
#define MI4_VOLUME_DATA_CREATOR_HPP 1
#include <memory>
#include <mi4/VolumeData.hpp>
namespace mi4
{
        template <typename T>
        class VolumeDataCreator
        {
        private:
                class Impl
                {
                private:
                        VolumeData<T>& _data; ///< Volume data.
                        T _value;      ///< Current value.
                private:
                        Impl ( const Impl& that ) = delete;
                        Impl ( Impl&& that ) = delete;
                        Impl& operator = ( const Impl& that ) = delete;
                        Impl& operator = ( Impl&& that ) = delete;
                public:
                        Impl ( VolumeData<T>& data, const T value ) : _data ( data ), _value ( value )
                        {
                                return;
                        }

                        VolumeData<T>& data ( void )
                        {
                                return this->_data;
                        }

                        T& value ( void )
                        {
                                return this->_value;
                        }
                };
                // Variables.
                std::unique_ptr<Impl> _impl;
        private:
                VolumeDataCreator ( const VolumeDataCreator& that ) = delete;
                VolumeDataCreator ( VolumeDataCreator&& that ) = delete;
                VolumeDataCreator& operator = ( const VolumeDataCreator& that ) = delete;
                VolumeDataCreator& operator = ( VolumeDataCreator&& that ) = delete;
        public:
                explicit VolumeDataCreator ( VolumeData<T>& data, const T value = T() ) : _impl ( new Impl ( data, value ) )
                {
                        this->fill();
                }

                ~VolumeDataCreator ( void ) = default;

                VolumeDataCreator<T>& setValue ( const T value )
                {
                        this->_impl->value() = value;
                        return *this;
                }

                T getValue ( void ) const
                {
                        return this->_impl->value();
                }

                VolumeDataCreator<T>& fill ( void )
                {
                        const auto& info = this->_impl->data().getInfo();
                        this->fillBlock ( info.getMin(), info.getMax() );
                        return *this;
                }

                inline VolumeDataCreator<T>& fillSphere ( const Point3i& p, const double rad )
                {
                        const auto& info = this->_impl->data().getInfo();
                        const auto rp = info.getPointInVoxelCeil ( Point3d ( rad, rad, rad ) + info.getOrigin() );
                        const auto radSqr = rad * rad;

                        for ( const auto& d : mi4::Range ( -rp, rp ) ) {
                                if ( info.getLengthSquared ( d )  <= radSqr ) {
                                        this->fillPoint ( p + d );
                                }
                        }

                        return *this;
                }

                inline VolumeDataCreator<T>& fillPoint ( const Point3i& p )
                {
                        auto& data = this->_impl->data();

                        if ( data.getInfo().isValid ( p ) ) {
                                data.set ( p, this->getValue() );
                        }

                        return *this;
                }

                inline VolumeDataCreator<T>& fillBlock ( const Point3i& bmin,  const Point3i& bmax )
                {
                        for ( const auto& p : mi4::Range ( bmin, bmax ) ) {
                                this->fillPoint ( p );
                        }

                        return *this;
                }
        };
}
#endif// MI_VOLUME_DATA_CREATOR_HPP
