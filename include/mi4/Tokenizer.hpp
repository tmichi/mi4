/**
 * @file Tokenizer.hpp
 * @author Takashi Michikawa <michikawa@acm.org>
 */
#ifndef MI4_TOKENIZER_HPP
#define MI4_TOKENIZER_HPP 1
#include <string>
#include <vector>
namespace mi4
{
        class Tokenizer
        {
        private:
                Tokenizer ( const Tokenizer& );
                Tokenizer& operator = ( const Tokenizer& );
                Tokenizer ( Tokenizer&& );
                Tokenizer& operator = ( Tokenizer&& );
        public:
                explicit Tokenizer ( const std::string& str, const std::string& delimiter = std::string ( " " ) )
                {
                        auto& token = this->_token;
                        token.clear();
                        std::string::size_type end = 0;
                        std::string line = str;

                        while ( end != std::string::npos ) {
                                end = line.find_first_of ( delimiter );

                                if ( line.substr ( 0, end ).length() > 0 ) {
                                        token.push_back ( line.substr ( 0, end ) );
                                }

                                line = line.substr ( end + 1 );
                        }

                        return;
                }
                ~Tokenizer ( void ) = default;

                size_t size ( void ) const
                {
                        return this->_token.size();
                }

                std::string get ( const size_t i ) const
                {
                        return this->_token.at ( i );
                }

                std::string toString ( void ) const
                {
                        std::string result;

                        for ( const auto s : this->_token ) {
                                if ( result.length() != 0 ) {
                                        result.append ( " " );
                                }

                                result.append ( s );
                        }

                        return result;
                }
        private:
                std::vector<std::string> _token;
        };

        int fuzzy_compare ( const std::string& str0, const std::string& str1 )
        {
                return Tokenizer ( str0 ).toString().compare ( Tokenizer ( str1 ).toString() );
        }


};
#endif//MI4_TOKENIZER_HPP
