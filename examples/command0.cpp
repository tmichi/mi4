#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <mi4/ProgramTemplate.hpp>

class ExampleProgram : public mi4::ProgramTemplate
{
private:
        std::string _input_file;
        std::string _output_file;
        double      _param0;
        int _a;
        int _b;
public:
        ExampleProgram ( void ) ;
        ~ExampleProgram ( void ) ;
        bool init ( const mi4::Argument& arg ) ;
        bool run  ( void );
        bool term ( void );
};

ExampleProgram::ExampleProgram ( void ) : mi4::ProgramTemplate(  )
{
        mi4::AttributeSet& attrSet = this->getAttributeSet();
        attrSet.createStringAttribute ( "-i", this->_input_file, "input file" ).setMandatory();
        attrSet.createStringAttribute ( "-o", this->_output_file, "output file" ).setMandatory();
        attrSet.createDoubleNumericAttribute ( "-d", this->_a, this->_b, "double" ).setMandatory();
        attrSet.createNumericAttribute ( "-p", this->_param0, "value" ).setMin ( -10 ).setMax ( 10 ).setMandatory().setOutRangeRejected();
        return;
}

ExampleProgram::~ExampleProgram ( void )
{
        std::cerr << "ExampleProgram::~ExampleProgram() was called." << std::endl;
        return;
}


bool
ExampleProgram::init ( const  mi4::Argument& arg )
{
        if ( ! this->getAttributeSet().parse ( arg ) ) {
                return false;
        }

        this->getAttributeSet().print(std::cerr);

        return true;
}

bool
ExampleProgram:: run ( void )
{
        std::cerr << "ExampleProgram::run() was called." << std::endl;
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
        ExampleProgram cmd;
        return  mi4::ProgramTemplate::execute ( cmd, arg );
}
