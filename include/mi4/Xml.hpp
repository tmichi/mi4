#ifndef MI4_XML_HPP
#define MI4_XML_HPP 1
#include <memory>
#include <list>
#include <map>
#include <string>
#include <iostream>
#include <sstream>

namespace mi4
{
        enum XmlNodeType {
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
                virtual std::string toString ( const size_t = 0 ) const = 0;
        };

        class XmlText : public XmlNode
        {
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

                std::string toString ( const size_t indent = 0 ) const
                {
                        return std::string ( indent * 4, ' ' ).append ( this->getText() ).append ( "\n" );
                }
        private:
                std::string _text;
        };

        class XmlElement : public XmlNode
        {
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
                        this->_attributes[key] = ss.str();//std::to_string(value);//ss.str();
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

                        if ( iter != this->_attributes.end() ) {
                                return iter->second;        // if key is found
                        } else {
                                return std::string();        // if key does not exist
                        }
                }

                std::list<std::string> getAttributeKeys ( void ) const
                {
                        std::list<std::string> keys;

                        for ( auto& iter : this->_attributes ) {
                                keys.push_back ( iter.first );
                        }

                        return keys;
                }

                const std::string& getName ( void ) const
                {
                        return this->_name;
                }

                std::string toString ( const size_t indent = 0 ) const
                {
                        auto& children = this->_children;
                        std::string space ( indent * 4, ' ' );
                        std::string str;
                        str.append ( space ).append ( "<" ).append ( this->getName() );

                        for ( auto& iter : this->_attributes ) {
                                str.append ( " " ).append ( iter.first ).append ( "=\"" ).append ( iter.second ).append ( "\"" );
                        }

                        str.append ( ">" ).append("`\n" );

                        for ( auto& iter : children ) {
                                str.append ( iter->toString ( indent + 1 ) );
                        }

                        str.append ( space ).append ( "</" ).append ( this->getName() ).append ( ">").append("\n" );
                        return str;
                }
        private:
                std::string _name; // element name
                std::list<std::unique_ptr<XmlNode> > _children; // child nodes.
                std::map<std::string, std::string> _attributes; // attributes
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
                        return std::string ( "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>" ).append("\n").append ( this->_root->toString() );
                }
        private:
                std::unique_ptr<XmlElement> _root;
        };
}
#endif// MI_XML_HPP
