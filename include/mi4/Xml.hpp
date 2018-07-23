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
        /*
        enum XmlNodeType : uint8_t {
                ElementNode,
                TextNode
        };
*/
        class XmlNode {
        private:
                XmlNode (const XmlNode& ) = delete;
                XmlNode (XmlNode&& ) = delete;
                void operator = (const XmlNode& ) = delete;
                void operator = (XmlNode&& ) = delete;
        protected:
                XmlNode (void ) = default;
        public:
                virtual ~XmlNode (void ) = default;
                //virtual XmlNodeType getType ( void ) const = 0;
                virtual bool isLeaf (void ) const = 0;
                virtual void write (std::ostream& os, const size_t = 0 ) = 0;
                virtual bool isElement (void) const
                {
                        return false;
                }
        };

        class XmlText : public XmlNode
        {
        private:
                std::string text_;
        public:
                XmlText (const std::string& text) : XmlNode(), text_(text)
                {
                }

                ~XmlText ( void ) = default;
/*
                XmlNodeType getType  ( void ) const
                {
                        return TextNode;
                }
*/
                bool isLeaf ( void ) const
                {
                        return true;
                }

                const std::string& getText ( void ) const
                {
                        return this->text_;
                }

                void write ( std::ostream& os, const size_t indent = 0 )
                {
                        os << std::string(indent, ' ') << this->getText() << std::endl;
                }
        };

        class XmlElement : public XmlNode
        {
        private:
                std::string name_; // element name
                std::list< std::unique_ptr< XmlNode > > children_; // child nodes.
                std::map< std::string, std::string > attributes_; // attributes
        public:
                XmlElement (const std::string& name) : XmlNode(), name_(name)
                {
                        return;
                }

                virtual ~XmlElement ( void ) = default;

                /*
                XmlNodeType getType ( void ) const
                {
                        return ElementNode;
                }
                 */

                bool isElement (void) const
                {
                        return true;
                }

                bool isLeaf (void) const
                {
                        return this->children_.size() == 0;
                }

                XmlElement& addChildElement ( const std::string& name )
                {
                        this->getChildren().emplace_back(new XmlElement(name));
                        return dynamic_cast<XmlElement&> ( *(this->getChildren().back().get()));
                }

                XmlText& addChildText ( const std::string& str )
                {
                        this->children_.emplace_back(new XmlText(str));
                        return dynamic_cast<XmlText&> ( *(this->getChildren().back().get()));
                }

                template <typename T>
                XmlElement& addAttribute ( const std::string& key, const T& value )
                {
                        std::stringstream ss;
                        ss << value;
                        this->attributes_[key] = ss.str();
                        return *this;
                }

                std::list< std::unique_ptr<XmlNode> >& getChildren ( void )
                {
                        return this->children_;
                }

                std::map< std::string, std::string >& getAttributes (void)
                {
                        return this->attributes_;
                }

                std::string getAttribute ( const std::string& key ) const
                {
                        return const_cast<XmlElement *>(this)->attributes_[key];
                }

                const std::string& getName ( void ) const
                {
                        return this->name_;
                }

                std::string toString ( const size_t indent = 0 ) const
                {
                        std::stringstream ss;
                        const_cast<XmlElement *>( this )->write(ss, indent);
                        return ss.str();
                }
        private:
                void write ( std::ostream& os, const size_t indent = 0 )
                {
                        os << std::string(indent, ' ') << "<" << this->getName();
                        for ( const auto& iter : this->getAttributes()) {
                                os << " " << iter.first << "=" << std::quoted ( iter.second );
                        }

                        if ( this->children_.empty()) {
                                os << "/>" << std::endl;
                        } else {
                                os << ">" << std::endl;
                                for ( const auto& iter : this->getChildren()) {
                                        iter->write(os, indent + 4);
                                }
                                os << std::string(indent, ' ') << "</" << this->getName() << ">" << std::endl;
                        }

                        return;
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
                XmlDocument (const std::string& name) : root_(new XmlElement(name))
                {
                        return;
                }

                virtual ~XmlDocument ( void ) = default;

                XmlElement& getRoot ( void )
                {
                        return *(this->root_);
                }

                std::string toString ( void ) const
                {
                        std::stringstream ss;
                        ss << "<?xml version=" << std::quoted("1.0") << " encoding=" << std::quoted("UTF-8") << " ?>" << std::endl;
                        ss << this->root_->toString(0);
                        return ss.str();
                }
        private:
                std::unique_ptr< XmlElement > root_;
        };
}
#endif// MI_XML_HPP








