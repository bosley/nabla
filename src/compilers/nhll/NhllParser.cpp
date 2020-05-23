#include "NhllParser.hpp"

#include <iostream>

namespace NABLA
{
namespace COMPILERS
{

    //
    //  ------------------------------------------- NHLL PARSER -------------------------------------------------
    //

    NhllParser::NhllParser(NhllLanguageIf & iface) : line_number(0), line_index(0), language_iface(&iface)
    {

    }

    // -----------------------------------------------
    //
    // -----------------------------------------------

    NhllParser::~NhllParser()
    {

    }

    // -----------------------------------------------
    //
    // -----------------------------------------------

    void NhllParser::parseLine(std::string line)
    {

    }



}
}