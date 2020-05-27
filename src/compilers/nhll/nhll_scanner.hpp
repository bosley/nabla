#ifndef __NHLLSCANNER_HPP__
#define __NHLLSCANNER_HPP__ 1

#if ! defined(yyFlexLexerOnce)
#include <FlexLexer.h>
#endif

#include "nhll_parser.tab.hh"
#include "location.hh"

namespace NHLL
{

   class NHLL_Driver;

class NHLL_Scanner : public yyFlexLexer{
public:
   
   NHLL_Scanner(std::istream *in, NHLL_Driver& driver) : yyFlexLexer(in), driver(driver)
   {
   };
   virtual ~NHLL_Scanner() {
   };

   //get rid of override virtual function warning
   using FlexLexer::yylex;

   virtual
   int yylex( NHLL::NHLL_Parser::semantic_type * const lval, 
              NHLL::NHLL_Parser::location_type *location );
   // YY_DECL defined in nhll_lexer.l
   // Method body created by flex in nhll_lexer.yy.cc


private:
   /* yyval ptr */
   NHLL::NHLL_Parser::semantic_type *yylval = nullptr;

   NHLL_Driver &driver;
};

} /* end namespace NHLL */

#endif /* END __NHLLSCANNER_HPP__ */
