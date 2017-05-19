#include <string>
#include <fstream>
#include <vector>

#include <mi4/ProgramTemplate.hpp>
#include <mi4/SystemInfo.hpp>


class ExampleProgram: public mi4::ProgramTemplate
{
private:
        std::string _input_file;
        std::string _output_file;
public:
        ExampleProgram ( void ) ;
        ~ExampleProgram ( void ) = default ;
        bool init ( const mi4::Argument& arg ) ;
        bool run  ( void );
        bool term ( void );
};

ExampleProgram::ExampleProgram ( void ) : mi4::ProgramTemplate ( "Program" )
{
        return;
}

bool
ExampleProgram::init ( const mi4::Argument& arg )
{
        mi4::AttributeSet& attrSet = this->getAttributeSet();
        attrSet.createStringAttribute ( "-i", this->_input_file ).setMandatory();
        attrSet.createStringAttribute ( "-o", this->_output_file ).setMandatory();

        if ( ! attrSet.parse ( arg ) ) {
                return false;
        }

        std::cerr << "-i:" << this->_input_file << std::endl;
        std::cerr << "-o:" << this->_output_file << std::endl;

        return true;
}

bool
ExampleProgram:: run ( void )
{
        std::cerr << "ExampleProgram::run() was called." << std::endl;
        std::vector<char> v ( 100000000, 0 );
        return true;
}

bool
ExampleProgram:: term ( void )
{
        std::cerr << "ExampleProgram::term() was called." << std::endl;
        return true;
}


int main ( int argc, char** argv )
{

        mi4::Argument arg ( argc, argv );
        ExampleProgram program;
        return mi4::ProgramTemplate::execute ( program, arg );

}
