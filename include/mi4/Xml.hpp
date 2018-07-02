#ifndef MI4_XML_HPP
#define MI4_XML_HPP 1

#include <memory>
#include <list>
#include <map>
#include <string>
#include <iostream>
#include <iomanip>
#include <sstream>

namespace mi4
{
        enum XmlNodeType : uint8_t {
                InvalidNode,
                ElementNode,
                TextNode
        };

        class XmlNode
        {
        private:
                XmlNode ( const XmlNode& ) = delete;
                XmlNode ( XmlNode&& ) = delete;
                void operator = ( const XmlNode& ) = delete;
                void operator = ( XmlNode&& ) = delete;
        protected:
                XmlNode ( void ) = default;
        public:
                virtual ~XmlNode ( void ) = default;
                virtual XmlNodeType getType ( void ) const = 0;
                virtual bool isLeaf ( void ) const = 0;
                virtual void write ( std::ostream& os, const size_t = 0 ) = 0;
        };

        class XmlText : public XmlNode
        {
        private:
                std::string _text;
        public:
                XmlText ( const std::string& text ) : XmlNode(),  _text ( text )
                {
                        return;
                }

                ~XmlText ( void ) = default;

                XmlNodeType getType  ( void ) const
                {
                        return TextNode;
                }

                bool isLeaf ( void ) const
                {
                        return true;
                }

                const std::string& getText ( void ) const
                {
                        return this->_text;
                }

                void write ( std::ostream& os, const size_t indent = 0 )
                {
                        os << std::string ( indent * 4, ' ' ) << this->getText() << std::endl;
                }
        };

        class XmlElement : public XmlNode
        {
        private:
                std::string _name; // element name
                std::list< std::unique_ptr< XmlNode > > _children; // child nodes.
                std::map< std::string, std::string > _attributes; // attributes
        public:
                XmlElement ( const std::string& name ) : XmlNode (), _name ( name )
                {
                        return;
                }

                virtual ~XmlElement ( void ) = default;

                virtual XmlNodeType getType ( void ) const
                {
                        return ElementNode;
                }

                virtual bool isLeaf ( void ) const
                {
                        return this->_children.size() == 0;
                }

                XmlElement& addChildElement ( const std::string& name )
                {
                        this->_children.emplace_back ( new XmlElement ( name )  );
                        return dynamic_cast<XmlElement&> ( * ( this->_children.back().get() ) );
                }

                XmlText& addChildText ( const std::string& str )
                {
                        this->_children.emplace_back ( new XmlText ( str ) );
                        return dynamic_cast<XmlText&> ( * ( this->_children.back().get() ) );
                }

                template <typename T>
                XmlElement& addAttribute ( const std::string& key, const T& value )
                {
                        std::stringstream ss;
                        ss << value;
                        this->_attributes[key] = ss.str();
                        return *this;
                }

                std::list< std::unique_ptr<XmlNode> >& getChildren ( void )
                {
                        return this->_children;
                }

                const std::map<std::string, std::string>& getAttributes ( void ) const
                {
                        return this->_attributes;
                }

                std::string getAttribute ( const std::string& key ) const
                {
                        const auto& iter = this->_attributes.find ( key );
                        return (iter != this->_attributes.end()) ? iter->second : std::string();
                }

                const std::string& getName ( void ) const
                {
                        return this->_name;
                }

                std::string toString ( const size_t indent = 0 ) const
                {
                        std::stringstream ss;
                        const_cast<XmlElement*> ( this )->write ( ss, 0 );
                        return ss.str();
                }
        private:
                void write ( std::ostream& os, const size_t indent = 0 )
                {
                        os << std::string ( indent * 4, ' ' ) << "<" << this->getName() ;
                        for ( const auto& iter : this->_attributes ) {
                                os << " " << iter.first << "=" << std::quoted ( iter.second );
                        }
                        os << (this->_children.empty() ? "/>" : ">") << std::endl;
                        //
                        if ( !this->_children.empty()) {
                                for ( const auto& iter : this->_children ) {
                                        iter->write(os, indent + 1);
                                }
                                os << std::string ( indent * 4, ' ' ) << "</" << this->getName() << ">" << std::endl;
                        }

                        return ;
                }
        };

        class XmlDocument
        {
        private:
                XmlDocument ( const XmlDocument& that )  = delete;
                XmlDocument ( XmlDocument&& that )  = delete;
                void operator = ( const XmlDocument& that )  = delete;
                void operator = ( XmlDocument&& that )  = delete;
        public:
                XmlDocument ( const std::string& name ) : _root ( new  XmlElement ( name ) )
                {
                        return;
                }

                virtual ~XmlDocument ( void ) = default;

                XmlElement& getRoot ( void )
                {
                        return * ( this->_root );
                }

                std::string toString ( void ) const
                {
                        std::stringstream ss;
                        ss << "<?xml version=" << std::quoted("1.0") << "  encoding=" << std::quoted(
                                "UTF-8") << " ?>" << std::endl;
                        ss << this->_root->toString();
                        return ss.str();
                }
        private:
                std::unique_ptr<XmlElement> _root;
        };
}
#endif// MI_XML_HPP
