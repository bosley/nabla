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

   std::vector<std::string> expression_builder;

   std::string pullExpr()
   {
      std::string a = "";
      for(auto & i : expression_builder)
      {
         a  = a + i;
      }
      expression_builder.clear();
      return a;
   }

#undef yylex
#define yylex scanner.yylex
}

%define api.value.type variant
%define parse.assert

%type<std::string> expression;
%type<std::string> term;
%type<std::string> factor;
%type<std::string> primary;

%token FUNC_DECL USE SET

%token <std::string> STRING_LITERAL
%token <std::string> INTEGER_LITERAL
%token <std::string> REAL_LITERAL
%token <std::string> IDENTIFIER

%token               END    0     "end of file"

%locations

%%

prog_option
   :  stmt | function_stmt | END;

expression
    : term                 { /* All of these need to be put into a tree so we can parse the expression */ }
    | expression '+' term  { }
    | expression '-' term  { }
    ;

term
    : factor               { }
    | term '*' factor      { }
    | term '/' factor      { }
    | term '%' factor      { }
    ;

factor
    : primary              { }
    | '-' factor           { }
    | '+' factor           { }
    ;

primary
    : IDENTIFIER           { }
    | INTEGER_LITERAL      { }
    | REAL_LITERAL         { }
    | '(' expression ')'   { }
    ;

stmt
   : stmt stmt
   | use_stmt
   | set_stmt
   ;

function_stmt
   : function_stmt function_stmt
   | function_decl
   ;

use_stmt
   : USE '(' STRING_LITERAL ')'                    { /* Make a 'use' struct, populate, and add to a list*/ }
   | USE '(' STRING_LITERAL ',' STRING_LITERAL ')' { /* Make a 'use' struct, populate, and add to a list*/ }
   ;

set_stmt
   : SET '(' IDENTIFIER ',' expression ')'         { std::cout << "set " << $3 << " to " << pullExpr() << std::endl; }
   ;

block 
   : '{' stmt '}' { /* std::cout << "BLOCK\n"; */ } 
   ;

recv_paramaters 
   : IDENTIFIER                                    { /* std::cout << "ID : " << $1 << std::endl; */}
   | IDENTIFIER ',' recv_paramaters                { /* std::cout << "ID : " << $1 << std::endl; */}
   ;

function_decl
   : FUNC_DECL '(' IDENTIFIER ',' '[' ']' ',' block ')'                 { /* Function Decl */ }
   | FUNC_DECL '(' IDENTIFIER ',' '[' recv_paramaters ']' ',' block ')' { /* Function Decl */ }
   ;


%%

void NHLL::NHLL_Parser::error( const location_type &l, const std::string &err_message )
{
   std::cerr << "Error: " << err_message << " at " << l << "\n";
}
