#ifndef __NHLLDRIVER_HPP__
#define __NHLLDRIVER_HPP__ 1

#include <string>
#include <cstddef>
#include <istream>

#include "nhll_scanner.hpp"
#include "nhll_parser.tab.hh"

namespace NHLL{

class NHLL_Driver
{
public:
   NHLL_Driver() = default;

   virtual ~NHLL_Driver();
   
   /** 
    * parse - parse from a file
    * @param filename - valid string with input file
    */
   void parse( const char * const filename );
   /** 
    * parse - parse from a c++ input stream
    * @param is - std::istream&, valid input stream
    */
   void parse( std::istream &iss );

   std::ostream& print(std::ostream &stream);

private:

   void parse_helper( std::istream &stream );

   NHLL::NHLL_Parser  *parser  = nullptr;
   NHLL::NHLL_Scanner *scanner = nullptr;
};

} /* end namespace NHLL */
#endif /* END __NHLLDRIVER_HPP__ */
