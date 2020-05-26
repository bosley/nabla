%{
/* C++ string header, for string ops below */
#include <string>
#include <iostream>

#include "nhll_driver.hpp"

/* Implementation of yyFlexScanner */ 
#include "nhll_scanner.hpp"

#undef  YY_DECL
#define YY_DECL int NHLL::NHLL_Scanner::yylex( NHLL::NHLL_Parser::semantic_type * const lval, NHLL::NHLL_Parser::location_type *loc )

/* typedef to make the returns for the tokens shorter */
using token = NHLL::NHLL_Parser::token;

/* define yyterminate as this instead of NULL */
#define yyterminate() return( token::END )

/* msvc2010 requires that we exclude this header file. */
#define YY_NO_UNISTD_H

/* update location on matching */
#define YY_USER_ACTION loc->step(); loc->columns(yyleng);

%}

%option debug
%option nodefault
%option yyclass="NHLL::NHLL_Scanner"
%option noyywrap
%option c++

%%

%{          /** Code executed at the beginning of yylex **/
            yylval = lval;
%}

"let"       { return( token::LET );  }
"use"       { return( token::USE );  }
"as"        { return( token::AS  );  }
"call"      { return( token::CALL  ); }
"pcall"     { return( token::PCALL ); }

"def"       { return( token::FUNC_DECL); }
"->"        { return( token::RET_ARROW); }
"int"       { return( token::INT      ); }
"real"      { return( token::REAL     ); }
"nil"       { return( token::NIL      ); }
"str"       { return( token::STR      ); }

"while"     { return( token::WHILE ); }
"loop"      { return( token::LOOP  ); }
"break"     { return( token::BREAK ); }

"lte"       { return( token::LTE ); }
"gte"       { return( token::GTE ); }
"gt"        { return( token::GT  ); }
"lt"        { return( token::LT  ); }
"ne"        { return( token::NE  ); }
"eq"        { return( token::EQ  ); }

"/*"(.|\n)*?"*/" {}

[ \t]+      { /* Eat WS */  }

[\n]+       { loc->lines(); }

"//"[^\n]*  { /* comment */ }


\"[^\"]*\"  {
               yylval->build< std::string >( yytext );
               return( token::STRING_LITERAL );
            }

[a-zA-Z_]+  { 
               yylval->build< std::string >( yytext );
               return( token::IDENTIFIER ); 
            }

[0-9]+    {
               yylval->build< std::string >( yytext );
               return ( token::INTEGER_LITERAL );
            }

-?[0-9]+\.[0-9]+ {
               yylval->build< std::string >( yytext );
               return ( token::REAL_LITERAL );
            }


.           { /* Allow chars to slip in  */ return yytext[0]; } 


%%


