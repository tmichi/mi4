#ifndef MI_ROUTINE_HPP
#define MI_ROUTINE_HPP 1
#include <iostream>
#include <string>
namespace mi4
{
        class Routine
        {
        private:
                Routine ( const Routine& that );
                void operator = ( const Routine& that );
                Routine ( Routine&& that );
                void operator = ( Routine&& that );
        private:
                const std::string _name;
                bool _status;
        protected:
                explicit Routine ( const std::string& name ) : _name ( name ), _status ( true )
                {
                        std::cerr << "Starting " << this->_name << " routine ... ";
                        return;
                }
        public:
                virtual ~Routine ( void )
                {
                        std::cerr << std::endl;

                        if ( this->check() ) {
                                std::cerr << this->_name << " done" << std::endl;
                        } else {
                                std::cerr << this->_name << " failed" << std::endl;
                        }
                }

                Routine* getInstance ( void )
                {
                        return this;
                }

                static bool run ( Routine* routine )
                {
                        if ( !routine->check() ) {
                                return false;
                        }

                        return routine->run_main_routine();
                }
        protected:
                virtual bool run_main_routine ( void )
                {
                        this->set_failed();
                        return false; //always
                }

                bool check ( void ) const
                {
                        return this->_status;
                }

                void set_failed ( void )
                {
                        this->_status = false;
                }
        };
}

#endif // MI4_ROUTINE_HPP
