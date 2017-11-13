/**
 * @file Timer.hpp
 * @author Takashi Michikawa <michikawa@acm.org>
 */

#ifndef MI4_TIMER_HPP
#define MI4_TIMER_HPP 1

#include <chrono>
#include <sstream>
#include <iomanip>
#include <iostream>

namespace mi4
{
        /**
         * @class Timer Timer.hpp <mi/Timer.hpp>
         * @brief Timer object.
         */
        class Timer
        {
        public:
                enum time_format {
                        TIME_MILI_SECOND,
                        TIME_SECOND, ///< Seconds.
                        TIME_MINUTE, ///< Minutes.
                        TIME_HOUR,   ///< Hours.
                        TIME_AUTO    ///< Default.
                };
        private:
                typedef std::chrono::high_resolution_clock clock;
                typedef clock::time_point clock_t;

                Timer ( const Timer& that ) = delete;
                Timer& operator = ( const Timer& that ) = delete;
                Timer ( Timer&& that ) = delete;
                Timer& operator = ( Timer&& that ) = delete;
        public:
		/**
		 * @param [in] key keyword of the timer. 
		 * @param [in] isAutoPrint set true when the duration is printed automatically.
		 */
		explicit Timer ( const std::string& key = std::string ( "time" ), const bool isAutoPrint = true ) : _key ( key ), _isAutoPrint ( isAutoPrint ), _begin ( clock::now() ), _time ( -1 )
                {
                        return;
                }

                ~Timer ( void )
                {
                        if ( this->_isAutoPrint ) {
                                if ( this->_time < 0 ) {
                                        this->end();
                                }

                                std::cerr << this->toString() << std::endl;
                        }
                }

		void stop ( void ) {
                        this->_time = std::chrono::duration_cast<std::chrono::milliseconds> ( clock::now() - this->_begin ).count() * 0.001;
		}
                void end ( void )
			{
		       return this->stop();
                        return;
                }

                std::string key ( void ) const
                {
                        return this->_key;
                }

                double time ( const time_format format = TIME_SECOND ) const
                {
                        return this->_time * this->get_scale ( format );
                }

                std::string toString ( const int digit = 6, const time_format format = TIME_AUTO ) const
                {
                        if ( format == TIME_AUTO ) {
                                return this->toString ( digit, this->estimate_format ( this->_time ) );
                        } else {
                                std::stringstream ss;
                                ss << this->key() << ": \t" << std::setprecision ( digit ) << this->time ( format )
                                   << " " << this->get_format_string ( format );
                                return ss.str();
                        }
                }
        private:
                inline double get_scale ( const time_format format ) const
                {
                        if      ( format == TIME_HOUR   ) {
                                return 1.0 / 3600.0;
                        } else if ( format == TIME_MINUTE ) {
                                return 1.0 / 60.0;
                        } else {
                                return 1.0;
                        }
                }

                inline time_format estimate_format ( const double t ) const
                {
                        if      ( t < 60   ) {
                                return TIME_SECOND;
                        } else if ( t < 3600 ) {
                                return TIME_MINUTE;
                        } else {
                                return TIME_HOUR;
                        }
                }

                inline std::string get_format_string ( const time_format format ) const
                {
                        if      ( format == TIME_SECOND ) {
                                return std::string ( "[s]" );
                        } else if ( format == TIME_MINUTE ) {
                                return std::string ( "[m]" );
                        } else if ( format == TIME_HOUR   ) {
                                return std::string ( "[h]" );
                        } else {
                                return std::string ( "[?]" );
                        }
                }
        private:
                const std::string _key; // key of timer
                const bool _isAutoPrint;
                const clock_t _begin;
                double _time; // seconds.
        };
};
#endif //MI_TIMER_HPP
