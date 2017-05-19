


#ifndef MI4_XML_HPP
#define MI4_XML_HPP 1
#include <memory>
#include <string>
#include <list>
#include <map>
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
                virtual std::string toString ( const unsigned int = 0 ) const = 0;
        protected:
                std::string set_space ( const unsigned int indent ) const
                {
                        return std::string ( indent * 4, ' ' );
                }
        };

        class XmlText : public XmlNode
        {
        public:
                XmlText ( const std::string& text ) : _text ( text )
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

                std::string toString ( const unsigned int indent = 0 ) const
                {
                        std::string str = this->set_space ( indent ) ;
                        str.append ( this->getText() );
                        return str;
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
                        std::unique_ptr<XmlElement> elem ( new XmlElement ( name ) );
                        this->_children.push_back ( std::move ( elem ) );
                        return dynamic_cast<XmlElement&> ( * ( this->_children.back().get() ) );
                }

                XmlText& addChildText ( const std::string& str )
                {
                        std::unique_ptr<XmlText> elem ( new XmlText ( str ) );
                        this->_children.push_back ( std::move ( elem ) );
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

                        if ( iter != this->_attributes.end() ) {
                                return iter->second;
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

                virtual std::string toString ( const unsigned int indent = 0 ) const
                {
                        std::stringstream ss;
                        // header
                        ss << this->set_space ( indent );
                        ss << "<" << this->getName();

                        for ( auto& iter : this->_attributes ) {
                                ss << " " << iter.first << "=" << "\"" << iter.second << "\"";
                        }

                        if ( this->_children.size() == 0 ) {
                                ss << " />" << std::endl;
                        } else {
                                ss << ">" << std::endl;

                                for ( auto& iter : this->_children ) {
                                        ss << iter->toString ( indent + 1 ) ;
                                }

                                ss << this->set_space ( indent );
                                ss << "</" << this->getName() << ">" << std::endl;
                        }

                        return ss.str();
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
                        std::string doc ( "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n" );
                        doc.append ( this->_root->toString() );
                        return doc;
                }
        private:
                std::unique_ptr<XmlElement> _root;
        };
}
#endif// MI_XML_HPP