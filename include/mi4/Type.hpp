#ifndef MI4_TYPE_HPP
#define MI4_TYPE_HPP 1

#include <string>
#include <typeinfo>
#include <iostream>
#include <map>

namespace mi4 {
        std::string get_type_string (const std::type_info& type)
        {
                static std::map< std::string, std::string > tables = {{typeid(uint8_t).name(),  "uchar"},
                                                                      {typeid(int8_t).name(),   "char"},
                                                                      {typeid(uint16_t).name(), "ushort"},
                                                                      {typeid(int16_t).name(),  "short"},
                                                                      {typeid(uint32_t).name(), "uint"},
                                                                      {typeid(int32_t).name(),  "int"},
                                                                      {typeid(float).name(),    "float"},
                                                                      {typeid(double).name(),   "double"},
                                                                      {typeid(char).name(),     "char"}};
                return tables[type.name()];
        }
}
#endif //MI4_TYPE_HPP