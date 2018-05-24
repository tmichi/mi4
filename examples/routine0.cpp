#include "../include/mi4/Routine.hpp"

class SuccessRoutine : public mi4::Routine
{
public:
        SuccessRoutine ( void ) : mi4::Routine ( "success" )
        {
                return;
        }

protected:
        bool run_main_routine ( void )
        {
                return true;
        }
};

class FailureRoutine : public mi4::Routine
{
public:
        FailureRoutine ( void ) : mi4::Routine ( "failure" )
        {
                this->add_error_message ( "invalid error" );
                return;
        }

protected:
        bool run_main_routine ( void )
        {
                return true;
        }
};

int main ( int argc, char** argv )
{
        if ( ! SuccessRoutine().run() ) {
                std::cerr << "error" << std::endl;
        } else {
                std::cerr << "ok" << std::endl;
        }

        if ( ! FailureRoutine().run() ) {
                std::cerr << "expected error. ok" << std::endl;
        }

        return 0;
}
