/**
 * @file  ProgramTemplate.hpp
 * @author Takashi Michikawa <michikawa@acm.org>
 */
#ifndef MI_PROGRAM_TEMPLATE_HPP
#define MI_PROGRAM_TEMPLATE_HPP 1

#include <string>
#include <deque>
#include <list>
#include <iostream>
#include <vector>
#include <memory>

namespace mi4
{
        namespace parser
        {
                // parse string to numeric value or something.
                template < typename T >
                inline T parse ( const std::string& str );

                template <>
                inline char parse (const std::string& str)
                {
                        return static_cast<char> ( std::stoi(str));
                }
                template <>
                inline int8_t parse ( const std::string& str )
                {
                        return static_cast<int8_t> ( std::stoi ( str ) );
                }
                template <>
                inline uint8_t parse ( const std::string& str )
                {
                        return static_cast<uint8_t> ( std::stoi ( str ) );
                }
                template <>
                inline int16_t parse ( const std::string& str )
                {
                        return static_cast<int16_t> ( std::stoi ( str ) );
                }
                template <>
                inline uint16_t parse ( const std::string& str )
                {
                        return static_cast<uint16_t> ( std::stoi ( str ) );
                }
                template <>
                inline int32_t parse ( const std::string& str )
                {
                        return std::stoi ( str );
                }
                template <>
                inline uint32_t parse ( const std::string& str )
                {
                        return std::stoul ( str );
                }
                template <>
                inline int64_t parse ( const std::string& str )
                {
                        return std::stoll ( str );
                }
                template <>
                inline uint64_t parse ( const std::string& str )
                {
                        return std::stoull ( str );
                }
                template <>
                inline float parse ( const std::string& str )
                {
                        return std::stof ( str );
                }
                template <>
                inline double parse ( const std::string& str )
                {
                        return std::stod ( str );
                }
                template <>
                inline std::string parse ( const std::string& str )
                {
                        return str;
                }
        }

        /**
         * @brief Argument object.
         */

        class Argument
        {
        private:
                Argument ( Argument&& ) = delete;
                Argument ( const Argument& ) = delete;
                void operator = ( const Argument& ) = delete;
                void operator = ( Argument&& ) = delete;
        public:
                /**
                 * @param[in] argc the number of arguments. use argc in main().
                 * @param[out] argv the arguments. use argv in main().
                 */
                explicit Argument (int argc = 0, char **argv = nullptr)
                {
                        for ( int i = 0; i < argc; ++i ) {
                                this->add ( argv[i] );
                        }
                }
                ~Argument ( void ) = default;

                int size ( void ) const
                {
                        return static_cast<int> ( this->argv_.size());
                }

                Argument& add ( const std::string& str )
                {
                        this->argv_.push_back(str);
                        return *this;
                }

                bool exist ( const std::string& key, const size_t offset = 0 ) const
                {
                        return ( this->find ( key, offset ) > -1 );
                }

                template < typename T >
                T get ( const std::string& key, const size_t offset = 1 ) const
                {
                        return (this->find(key, offset) == -1) ? T() : this->get< T >(this->find(key, offset));
                }

                template < typename T >
                T get ( const size_t idx ) const
                {
                        return parser::parse< T >(this->argv_.at(idx));
                }
        private:
                int find ( const std::string& key, const size_t offset = 0 ) const
                {
                        for ( size_t i = 0; i < this->size(); ++i ) {
                                if ( i + offset < this->size() && key.compare(this->get< std::string >(i)) == 0 ) {
                                        return i + offset;
                                }
                        }

                        return -1; // any arguments are not matched.
                }
        private:
                std::deque< std::string > argv_;
        };

        class Attribute
        {
        protected:
                enum ErrorCode : int8_t {
                        ATTRIBUTE_ERROR_OK = 0, ATTRIBUTE_ERROR_KEY_NOT_FOUND = -1, ATTRIBUTE_ERROR_VALUE_OUT_OF_RANGE = -2
                };
        private:
                const std::string key_;
                std::string _message;
                bool _isMandatory;
                bool _isHidden;
                Attribute::ErrorCode _errorCode;
        protected:
                explicit Attribute (const std::string& key = std::string("")) : key_(key), _message(std::string()), _isMandatory(false), _isHidden(false), _errorCode(ATTRIBUTE_ERROR_OK) {}
        public:
                virtual ~Attribute ( void ) = default;

                Attribute& setMessage (const std::string& message)
                {
                        this->_message = message;
                        return *this;
                }

                virtual Attribute& setMandatory ( void )
                {
                        this->_isMandatory = true;
                        return *this;
                }

                virtual Attribute& setHidden ( void )
                {
                        this->_isHidden = true;
                        return *this;
                }

                virtual bool parse ( const Argument& arg ) const = 0;

                virtual void printError ( void )
                {
                        switch ( this->get_error_code() ) {
                        case ATTRIBUTE_ERROR_OK :
                                break;

                        case ATTRIBUTE_ERROR_KEY_NOT_FOUND :
                                std::cerr << this->get_key() << " was not found." << std::endl;
                                break;

                        case ATTRIBUTE_ERROR_VALUE_OUT_OF_RANGE :
                                std::cerr << "key  " << this->get_key() << " is out-of-range." << std::endl;
                                break;

                                default : // never called.
                                std::cerr << "unknown error found." << std::endl;
                                break;
                        }

                        return;
                }

                virtual void print_usage ( void )
                {
                        if ( !this->is_hidden()) {
                                std::cerr << "\t" << this->key_ << ":\t" << this->_message << std::endl;
                        }

                        return;
                }

                virtual void print ( std::ostream& out )
                {
                        if ( !this->is_hidden()) {
                                out << "[\"" << this->get_key() << "\"] " << this->toString() << std::endl;
                        }

                        return;
                }
                virtual std::string toString (void) const
                {
                        return std::string();
                }
        protected:
                bool is_mandatory ( void ) const
                {
                        return this->_isMandatory;
                }

                bool is_hidden ( void ) const
                {
                        return this->_isHidden;
                }

                std::string get_key ( void ) const
                {
                        return this->key_;
                }

                Attribute::ErrorCode get_error_code ( void ) const
                {
                        return this->_errorCode;
                }
                void set_error_code (const Attribute::ErrorCode code)
                {
                        this->_errorCode = code;
                }
        };

        template < typename T >
        class NumericAttribute : public Attribute
        {
        private:
                T& _value;
                int _offset;
                T _minValue; ///< Minimum value.
                T _maxValue; ///< Maximum value.
                T _defaultValue;  ///< Default value.
                bool _isMinSet; ///< Minimum value is set.
                bool _isMaxSet; ///< Maximum value is set.
                bool _isOutRangeRejected; ///< Out rangevalue is rejected.
        public:
                NumericAttribute (const std::string& key, T& value, const int offset = 1) : Attribute(key), _value(value), _offset(offset), _minValue(T()), _maxValue(T()), _defaultValue(T()), _isMinSet(false), _isMaxSet(false), _isOutRangeRejected(false)
                {
                        return;
                }

                ~NumericAttribute ( void ) = default;

                bool parse ( const Argument& arg ) const
                {
                        T& value = this->_value;
                        if ( arg.exist(this->get_key(), this->_offset)) {
                                value = arg.get< T >(this->get_key(), this->_offset);
                                return const_cast<NumericAttribute< T > *> ( this )->clamp_value(value);
                        } else {
                                if ( this->is_mandatory() ) {
                                        const_cast<NumericAttribute< T > *> ( this )->set_error_code(ATTRIBUTE_ERROR_KEY_NOT_FOUND);
                                        return false;
                                } else {
                                        value = this->_defaultValue;
                                        return true;
                                }
                        }
                }


                NumericAttribute< T >& setMandatory (void)
                {
                        Attribute::setMandatory();
                        return *this;
                }

                NumericAttribute< T >& setHidden (void)
                {
                        Attribute::setHidden();
                        return *this;
                }

                NumericAttribute< T >& setMessage (const std::string& message)
                {
                        Attribute::setMessage(message);
                        return *this;
                }

                NumericAttribute< T >& setMin (const T minValue)
                {
                        this->_minValue = minValue;
                        this->_isMinSet = true;

                        if ( this->_isMaxSet ) {
                                this->swap_min_max ( this->_minValue, this->_maxValue );
                        }

                        this->clamp_value ( this->_defaultValue );
                        return *this;
                }

                NumericAttribute< T >& setMax (const T maxValue)
                {
                        this->_maxValue = maxValue;
                        this->_isMaxSet = true;

                        if ( this->_isMinSet ) {
                                this->swap_min_max ( this->_minValue, this->_maxValue );
                        }

                        this->clamp_value ( this->_defaultValue );
                        return *this;
                }

                NumericAttribute< T >& setDefaultValue (const T defaultValue)
                {
                        this->_defaultValue = defaultValue;
                        this->clamp_value ( this->_defaultValue );
                        return *this;
                }

                NumericAttribute< T >& setOutRangeRejected (void)
                {
                        this->_isOutRangeRejected = true;
                        return *this;
                }

        private:
                bool clamp_value ( T& value )
                {
                        if ( !this->clamp(value)) {
                                this->set_error_code ( ATTRIBUTE_ERROR_VALUE_OUT_OF_RANGE );
                                return false;
                        } else {
                                return true;
                        }
                }

                void swap_min_max ( T& minValue, T& maxValue )
                {
                        if ( maxValue < minValue ) {
                                std::swap(minValue, maxValue);
                        }
                }

                bool clamp ( T& value )
                {
                        if ( this->_isMinSet && value < this->_minValue ) {
                                if ( this->_isOutRangeRejected ) {
                                        return false;
                                }
                                value = this->_minValue;
                        }

                        if ( this->_isMaxSet && value > this->_maxValue ) {
                                if ( this->_isOutRangeRejected ) {
                                        return false;
                                }

                                value = this->_maxValue;
                        }

                        return true;
                }
        public:
                std::string toString ( void ) const
                {
                        return std::to_string ( this->_value );
                }
        };

        class StringAttribute : public Attribute
        {
        public:
                explicit StringAttribute (const std::string& key, std::string& value) : Attribute(key), _value(value), _defaultValue(std::string())
                {
                        return;
                }
                ~StringAttribute ( void ) = default;

                bool parse ( const Argument& arg ) const
                {
                        const std::string key = this->get_key();

                        if ( arg.exist ( key, 1 ) ) {
                                this->_value = arg.get< std::string >(key);
                                return true;
                        } else {
                                if ( this->is_mandatory() ) {
                                        const_cast<StringAttribute*> ( this )->set_error_code ( ATTRIBUTE_ERROR_KEY_NOT_FOUND );
                                        return false;
                                } else {
                                        this->_value = this->_defaultValue;
                                        return true;
                                }
                        }
                }

                StringAttribute& setDefaultValue (const std::string& defaultValue)
                {
                        this->_defaultValue = defaultValue;
                        return *this;
                }

                StringAttribute& setMandatory ( void )
                {
                        Attribute::setMandatory();
                        return *this;
                }
                StringAttribute& setHidden ( void )
                {
                        Attribute::setHidden();
                        return *this;
                }
                StringAttribute& setMessage ( const std::string& message )
                {
                        Attribute::setMessage(message);
                        return *this;
                }
        public:
                virtual std::string toString ( void ) const
                {
                        return this->_value;
                }
        private:
                std::string& _value;
                std::string _defaultValue; ///< Default value.
        };


        class BooleanAttribute : public Attribute
        {
        public:
                explicit BooleanAttribute (const std::string& key, bool& value) : Attribute(key), _value(value)
                {
                        return;
                }
                ~BooleanAttribute (void) = default;

                bool parse ( const Argument& arg ) const
                {
                        this->_value = arg.exist(this->get_key());
                        return /* always */ true;
                }

                BooleanAttribute& setMessage ( const std::string& message )
                {
                        Attribute::setMessage(message);
                        return *this;
                }
        public:
                virtual std::string toString ( void ) const
                {
                        return std::to_string ( static_cast<int> ( this->_value ) );
                }

        private:
                bool& _value;
        };

        template < typename T >
        class DoubleNumericAttribute : public Attribute
        {
        public:
                explicit DoubleNumericAttribute (const std::string& key, T& v0, T& v1) : Attribute(key), _attr0(new NumericAttribute< T >(key, v0, 1)), _attr1(new NumericAttribute< T >(key, v1, 2))
                {
                        return;
                }
                ~DoubleNumericAttribute ( void ) = default;

                bool parse ( const Argument& arg ) const
                {
                        return this->_attr0->parse ( arg ) && this->_attr1->parse ( arg );
                }
                DoubleNumericAttribute< T >& setMin (const T min0, const T min1)
                {
                        this->_attr0->setMin ( min0 );
                        this->_attr1->setMin ( min1 );
                        return *this;
                }
                DoubleNumericAttribute< T >& setMax (const T max0, const T max1)
                {
                        this->_attr0->setMax ( max0 );
                        this->_attr1->setMax ( max1 );
                        return *this;
                }
                DoubleNumericAttribute< T >& setDefaultValue (const T default0, const T default1)
                {
                        this->_attr0->setDefaultValue ( default0 );
                        this->_attr1->setDefaultValue ( default1 );
                        return *this;

                }
                DoubleNumericAttribute< T >& setOutRangeRejected (void)
                {
                        this->_attr0->setOutRangeRejected();
                        this->_attr1->setOutRangeRejected();
                        return *this;
                }
                DoubleNumericAttribute< T >& setMandatory (void)
                {
                        this->_attr0->setMandatory();
                        this->_attr1->setMandatory();
                        return *this;
                }
                DoubleNumericAttribute< T >& setHidden (void)
                {
                        Attribute::setHidden();
                        return *this;
                }
                DoubleNumericAttribute< T >& setMessage (const std::string& message)
                {
                        Attribute::setMessage(message);
                        return *this;
                }

                std::string toString ( void ) const
                {
                        return this->_attr0->toString() + " " + this->_attr1->toString();
                }
        private:
                std::unique_ptr< NumericAttribute< T > > _attr0;
                std::unique_ptr< NumericAttribute< T > > _attr1;
        };

        template < typename T >
        class TripleNumericAttribute : public Attribute
        {
        public:
                explicit TripleNumericAttribute (const std::string& key, T& v0, T& v1, T& v2) : Attribute(key), _attr0(new NumericAttribute< T >(key, v0, 1)), _attr1(new NumericAttribute< T >(key, v1, 2)), _attr2(new NumericAttribute< T >(key, v2, 3))
                {
                        return;
                }
                ~TripleNumericAttribute ( void ) = default;

                bool parse ( const Argument& arg ) const
                {
                        return this->_attr0->parse ( arg ) && this->_attr1->parse ( arg ) && this->_attr2->parse ( arg );
                }
                TripleNumericAttribute< T >& setMin (const T min0, const T min1, const T min2)
                {
                        this->_attr0->setMin ( min0 );
                        this->_attr1->setMin ( min1 );
                        this->_attr2->setMin ( min2 );
                        return *this;
                }
                TripleNumericAttribute< T >& setMax (const T max0, const T max1, const T max2)
                {
                        this->_attr0->setMax ( max0 );
                        this->_attr1->setMax ( max1 );
                        this->_attr2->setMax ( max2 );
                        return *this;
                }
                TripleNumericAttribute< T >& setDefaultValue (const T default0, const T default1, const T default2)
                {
                        this->_attr0->setDefaultValue ( default0 );
                        this->_attr1->setDefaultValue ( default1 );
                        this->_attr2->setDefaultValue ( default2 );
                        return *this;

                }
                TripleNumericAttribute< T >& setOutRangeRejected (void)
                {
                        this->_attr0->setOutRangeRejected();
                        this->_attr1->setOutRangeRejected();
                        this->_attr3->setOutRangeRejected();
                        return *this;
                }
                TripleNumericAttribute< T >& setMandatory (void)
                {
                        this->_attr0->setMandatory();
                        this->_attr1->setMandatory();
                        this->_attr2->setMandatory();
                        return *this;
                }
                TripleNumericAttribute< T >& setHidden (void)
                {
                        Attribute::setHidden();
                        return *this;
                }
                TripleNumericAttribute< T >& setMessage (const std::string& message)
                {
                        Attribute::setMessage(message);
                        return *this;
                }
                std::string toString ( void ) const
                {
                        std::string str;
                        str.append ( this->_attr0->toString() );
                        str.append ( " " );
                        str.append ( this->_attr1->toString() );
                        str.append ( " " );
                        str.append ( this->_attr2->toString() );
                        return str;
                }
        private:
                std::unique_ptr< NumericAttribute< T > > _attr0;
                std::unique_ptr< NumericAttribute< T > > _attr1;
                std::unique_ptr< NumericAttribute< T > > _attr2;
        };

        template < typename T >
        class ArrayNumericAttribute : public Attribute
        {
        public:
                ArrayNumericAttribute (const std::string& key, std::vector< T >& values) : Attribute(key)
                {
                        for ( size_t i = 0; i < values.size(); ++i ) {
                                std::unique_ptr< NumericAttribute< T > > attr(new NumericAttribute< T >(key, values[i], i + 1));
                                this->_attrs.push_back ( std::move ( attr ) );
                        }

                        return;
                }
                ~ArrayNumericAttribute ( void ) = default;


                bool parse ( const Argument& arg ) const
                {
                        for ( auto&& attr : this->_attrs ) {
                                if ( !attr->parse ( arg ) ) {
                                        return false;
                                }
                        }

                        return true;
                }

                ArrayNumericAttribute< T >& setMin (const T min0)
                {
                        for ( auto&& attr : this->_attrs ) {
                                attr->setMin ( min0 );
                        }

                        return *this;
                }
                ArrayNumericAttribute< T >& setMax (const T max0)
                {
                        for ( auto&& attr : this->_attrs ) {
                                attr->setMax ( max0 );
                        }

                        return *this;
                }
                ArrayNumericAttribute< T >& setDefaultValue (const T default0)
                {
                        for ( auto&& attr : this->_attrs ) {
                                attr->setDefaultValue ( default0 );
                        }

                        return *this;
                }
                ArrayNumericAttribute< T >& setOutRangeRejected (void)
                {
                        for ( auto&& attr : this->_attrs ) {
                                attr->setOutRangeRejected();
                        }

                        return *this;
                }
                ArrayNumericAttribute< T >& setMandatory (void)
                {
                        for ( auto&& attr : this->_attrs ) {
                                attr->setMandatory();
                        }

                        return *this;
                }
                ArrayNumericAttribute< T >& setHidden (void)
                {
                        Attribute::setHidden();
                        return *this;
                }
                ArrayNumericAttribute< T >& setMessage (const std::string& message)
                {
                        Attribute::setMessage(message);
                        return *this;
                }

                virtual std::string toString ( void ) const
                {
                        std::string str;

                        for ( auto&& attr : this->_attrs ) {
                                str.append ( attr->toString() ).append ( " " );
                        }

                        return str;
                }
        private:
                std::vector< std::unique_ptr< NumericAttribute< T > > > _attrs;
        };

        class AttributeSet : public Attribute
        {
        private:
                bool _isOr;
                std::list< std::unique_ptr< Attribute > > _attr;
        public:
                AttributeSet ( void ) : _isOr ( false )
                {
                        return;
                }
                virtual ~AttributeSet ( void ) = default;

                template < typename T >
                NumericAttribute< T >& createNumericAttribute (const std::string& key, T& value, const std::string& message = std::string())
                {
                        this->_attr.push_back(std::unique_ptr< Attribute >(new NumericAttribute< T >(key, value, 1)));
                        Attribute& attr = *(this->_attr.back());
                        attr.setMessage ( message );
                        return dynamic_cast<NumericAttribute< T >& > ( attr );
                }

                template < typename T >
                DoubleNumericAttribute< T >& createDoubleNumericAttribute (const std::string& key, T& value0, T& value1, const std::string& message = std::string())
                {
                        this->_attr.push_back(std::unique_ptr< Attribute >(new DoubleNumericAttribute< T >(key, value0, value1)));
                        Attribute& attr = *(this->_attr.back());
                        attr.setMessage ( message );
                        return dynamic_cast<DoubleNumericAttribute< T >& > ( attr );
                }

                template < typename T >
                TripleNumericAttribute< T >& createTripleNumericAttribute (const std::string& key, T& value0, T& value1, T& value2, const std::string& message = std::string())
                {
                        this->_attr.push_back(std::unique_ptr< Attribute >(new TripleNumericAttribute< T >(key, value0, value1, value2)));
                        Attribute& attr = *(this->_attr.back());
                        attr.setMessage ( message );
                        return dynamic_cast<TripleNumericAttribute< T >& > ( attr );
                }

                template < typename T >
                ArrayNumericAttribute< T >& createArrayNumericAttribute (const std::string& key, std::vector< T >& values, const std::string& message = std::string())
                {
                        this->_attr.push_back(std::unique_ptr< Attribute >(new ArrayNumericAttribute< T >(key, values)));
                        Attribute& attr = *(this->_attr.back());
                        attr.setMessage ( message );
                        return dynamic_cast<ArrayNumericAttribute< T >& > ( attr );
                }

                StringAttribute& createStringAttribute ( const std::string& key, std::string& value, const std::string& message = std::string() )
                {
                        this->_attr.push_back(std::unique_ptr< Attribute >(new StringAttribute(key, value)));
                        Attribute& attr = *(this->_attr.back());
                        attr.setMessage ( message );
                        return dynamic_cast<StringAttribute& > ( attr );
                }

                BooleanAttribute& createBooleanAttribute ( const std::string& key, bool& value, const std::string& message = std::string() )
                {
                        this->_attr.push_back(std::unique_ptr< Attribute >(new BooleanAttribute(key, value)));
                        Attribute& attr = *(this->_attr.back());
                        attr.setMessage ( message );
                        return dynamic_cast<BooleanAttribute& > ( attr );
                }


                bool parse ( const Argument& arg ) const
                {
                        bool result = this->is_and();

                        for ( auto& iter : this->_attr ) {
                                const bool r0 = iter->parse ( arg );

                                if ( this->is_and() ) {
                                        result &= r0;
                                } else {
                                        result |= r0;
                                }
                        }

                        if ( !result ) {
                                std::cerr << "Error" << std::endl;
                                const_cast<AttributeSet*> ( this )->printError();
                        }

                        return result;
                }

                void printError ( void )
                {
                        for ( auto&& iter : this->_attr ) {
                                iter->printError();
                        }

                        return;
                }

                void printUsage ( const std::string cmdStr )
                {
                        std::cerr << "Usage : " << cmdStr << " [OPTIONS]" << std::endl << std::endl << "OPTIONS:" << std::endl;
                        this->print_usage();
                }

                void setOr ( void )
                {
                        this->_isOr = true;
                        return;
                }

                void print_usage ( void )
                {
                        for ( auto&& iter : this->_attr ) {
                                iter->print_usage();
                        }
                }

                void print ( std::ostream& out )
                {
                        for ( auto&& iter : this->_attr ) {
                                iter->print ( out );
                        }
                }
        private:
                bool is_and ( void ) const
                {
                        return ! ( this->_isOr );
                }
        };

        /**
        * @brief Template for CUI-based command.
        */
        class ProgramTemplate
        {
        private:
                ProgramTemplate ( const ProgramTemplate& ) = delete;
                void operator = ( const ProgramTemplate& ) = delete;
                ProgramTemplate ( ProgramTemplate&& ) = delete;
                void operator = ( ProgramTemplate&& ) = delete;
        public:
                explicit ProgramTemplate (const std::string& cmdStr = "a.out") : cmdStr_(cmdStr), attr_(new AttributeSet()), isDebugModeOn_(false)
                {
                        return;
                }
                virtual ~ProgramTemplate ( void ) = default;

        protected:
                virtual bool init ( const Argument& arg ) = 0;
                virtual bool run (void) = 0;
                virtual bool term ( void ) = 0;

                AttributeSet& getAttributeSet ( void )
                {
                        return *(this->attr_);
                }

                bool isDebugMode ( void ) const
                {
                        return this->isDebugModeOn_;
                }

        private:
                void set_debug_mode_on ( void )
                {
                        this->isDebugModeOn_ = true;
                }

                void usage ( void )
                {
                        this->getAttributeSet().printUsage(this->cmdStr_);
                }
        public:
                static int execute ( ProgramTemplate& cmd, Argument& arg )
                {
                        if ( arg.exist ( "--debug" ) ) {
                                cmd.set_debug_mode_on();
                                std::cerr << "debug mode on." << std::endl;
                        }

                        if ( arg.exist ( "--help" ) ) {
                                cmd.usage();
                                return 2;
                        }

                        if ( !cmd.init ( arg ) ) {
                                std::cerr << "initialization failed." << std::endl;
                                cmd.usage();
                                return -1;
                        } else if ( !cmd.run()) {
                                std::cerr << "main routine failed." << std::endl;
                                return -2;
                        } else if ( !cmd.term() ) {
                                std::cerr << "termination failed." << std::endl;
                                return -3;
                        } else {
                                return 0; // SUCCESS
                        }
                }
        private:
                std::string cmdStr_;
                std::unique_ptr< AttributeSet > attr_;
                bool isDebugModeOn_;
        };//class ProgramTemplate
}//namespace mi
#endif // MI4_PROGRAM_TEMPLATE_HPP
