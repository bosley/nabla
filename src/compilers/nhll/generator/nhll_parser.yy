%skeleton "lalr1.cc"
%require  "3.0"
%debug 
%defines 
%define api.namespace {NHLL}

%define api.parser.class {NHLL_Parser}

%code requires{
   namespace NHLL {
      class NHLL_Driver;
      class NHLL_Scanner;
   }

// The following definitions is missing when %locations isn't used
# ifndef YY_NULLPTR
#  if defined __cplusplus && 201103L <= __cplusplus
#   define YY_NULLPTR nullptr
#  else
#   define YY_NULLPTR 0
#  endif
# endif

}

%parse-param { NHLL_Scanner  &scanner  }
%parse-param { NHLL_Driver  &driver  }

%code{
   #include <iostream>
   #include <cstdlib>
   #include <fstream>
   #include <stdint.h>
   
   /* include for all driver functions */
   #include "nhll_driver.hpp"

#undef yylex
#define yylex scanner.yylex
}

%define api.value.type variant
%define parse.assert

%type<std::string> expression;
%type<std::string> ope;
%type<std::string> primary;
%type<std::string> identifiers;

%token FUNC_DECL USE SET CALL PCALL

%token <std::string> STRING_LITERAL
%token <std::string> INTEGER_LITERAL
%token <std::string> REAL_LITERAL
%token <std::string> IDENTIFIER

%token               END    0     "end of file"

%locations

%%

prog_option
   :  stmt 
   | function_stmt 
   | END
   ;

identifiers
   :  IDENTIFIER                 { $$ = $1;      } 
   |  IDENTIFIER '.' identifiers { $$ = $1 + "." + $3; }
   ;

expression
    : primary                    { $$ = $1; }
    | expression ope expression  { $$ = std::string( $1 + $2 + $3); }
    | ope primary                { $$ = std::string( $1 + $2); }
    ;

ope
   : '+' { $$ = "+"; }
   | '*' { $$ = "*"; }
   | '-' { $$ = "-"; }
   | '/' { $$ = "/"; }
   | '^' { $$ = "^"; }
   ;

primary
    : IDENTIFIER           { $$ = $1; }
    | INTEGER_LITERAL      { $$ = $1; }
    | REAL_LITERAL         { $$ = $1; }
    | '(' expression ')'   { $$ = std::string( "(" + $2 + ")"); }
    ;

stmt
   : stmt stmt
   | use_stmt
   | set_stmt
   | call_stmt
   ;

function_stmt
   : function_stmt function_stmt
   | function_decl
   ;

use_stmt
   : USE '(' STRING_LITERAL ')'                    { /* Make a 'use' struct, populate, and add to a list*/ std::cout << "use "     << std::endl; }
   | USE '(' STRING_LITERAL ',' STRING_LITERAL ')' { /* Make a 'use' struct, populate, and add to a list*/ std::cout << "use as " << std::endl; }
   ;

set_stmt
   : SET '(' identifiers ',' expression ')'         { std::cout << "set " << $3 << " to expr: " << $5 << std::endl; }
   ;

call_stmt
   : CALL '(' identifiers ')'                              { std::cout << "Empty  call"  << std::endl; }
   | CALL '(' identifiers ',' '[' send_paramaters ']' ')'  { std::cout << "Filled call"  << std::endl; }
   | PCALL '(' identifiers ')'                             { std::cout << "Empty  pcall" << std::endl; }
   | PCALL '(' identifiers ',' '[' send_paramaters ']' ')' { std::cout << "Filled pcall" << std::endl; }
   ;

block 
   : '{' stmt '}' { /* std::cout << "BLOCK\n"; */ } 
   ;

recv_paramaters 
   : IDENTIFIER                                    { /* std::cout << "ID : " << $1 << std::endl; */}
   | IDENTIFIER ',' recv_paramaters                { /* std::cout << "ID : " << $1 << std::endl; */}
   ;

send_paramaters 
   : identifiers                                    { /* std::cout << "ID : " << $1 << std::endl; */}
   | identifiers ',' send_paramaters                { /* std::cout << "ID : " << $1 << std::endl; */}
   ;

function_decl
   : FUNC_DECL '(' IDENTIFIER ',' block ')'                             { /* Function Decl */ std::cout << "FUNC DECL: NO PARAM" << std::endl;}
   | FUNC_DECL '(' IDENTIFIER ',' '[' recv_paramaters ']' ',' block ')' { /* Function Decl */ std::cout << "FUNC DECL: PARAMS"   << std::endl;}
   ;


%%

void NHLL::NHLL_Parser::error( const location_type &l, const std::string &err_message )
{
   std::cerr << "Error: " << err_message << " at " << l << "\n";
}
