#ifndef MI4_ROUTINE_HPP
#define MI4_ROUTINE_HPP 1
#include <iostream>
#include <string>
#include <deque>
namespace mi4
{
        /**
         *
         *
         */
        class Routine
        {
        private:
                Routine ( const Routine& that ) = delete;
                void operator = ( const Routine& that ) = delete;
                Routine ( Routine&& that ) = delete;
                void operator = ( Routine&& that ) = delete;
        private:
                const std::string _name;
                bool _status;
                std::deque<std::string> _messages;
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
                                std::cerr << " done." << std::endl;
                        } else {
                                std::cerr << " failed." << std::endl;

                                for ( auto& s : this->_messages ) {
                                        std::cerr << s << std::endl;
                                }
                        }
                }
                bool run ( void )
                {
                        if ( !this->check() ) {
                                return false;
                        }

                        return this->run_main_routine();
                }

        protected:
                virtual bool run_main_routine ( void )
                {
                        this->add_error_message ( "Routine::run_main_routine() called. Declare the method in the subclass." );
                        return false; //always
                }
                void add_error_message ( const std::string& str )
                {
                        this->add_message("[error]", str);
                        this->_status = false;
                        return;
                }
                void add_warning_message (const std::string& str)
                {
                        this->add_message("[warning]", str);
                        return;
                }
        private:
                void add_message (const std::string& header, const std::string& str)
                {
                        this->_messages.push_back(header + this->_name + str);
                }
                bool check (void) const
                {
                        return this->_status;
                }
        };
}
#endif // MI4_ROUTINE_HPP
