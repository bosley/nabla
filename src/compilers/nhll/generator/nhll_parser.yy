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
      class NhllFunction;
      class NhllElement;
      class ConditionalExpression;
      enum class Conditionals;
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
   #include <memory>
   #include <vector>
   #include <iterator>
   
   #include "nhll.hpp"
   #include "nhll_driver.hpp"

   std::vector< std::vector<NHLL::NhllElement*> > element_list;

#undef yylex
#define yylex scanner.yylex
}

%define api.value.type variant
%define parse.assert

%type<std::string> expression;
%type<std::string> ope;
%type<std::string> primary;
%type<std::string> identifiers;
%type<NHLL::ConditionalExpression*> condexpr;
%type<int> conditional;

%type<NHLL::NhllElement*> stmt;
%type<NHLL::NhllElement*> use_stmt;
%type<NHLL::NhllElement*> set_stmt;
%type<NHLL::NhllElement*> call_stmt;
%type<NHLL::NhllElement*> while_stmt;
%type<NHLL::NhllElement*> function_stmt;

%type<std::vector<NhllElement*>> multiple_statements;
%type<std::vector<NhllElement*>> block;
%type<std::vector<NhllElement*>> function_statements;

%type<std::vector<std::string>> recv_paramaters;
%type<std::vector<std::string>> send_paramaters;

%token FUNC_DECL USE SET CALL PCALL WHILE
%token LTE GTE LT GT EQ NE 

%token <std::string> STRING_LITERAL
%token <std::string> INTEGER_LITERAL
%token <std::string> REAL_LITERAL
%token <std::string> IDENTIFIER


%token               END    0     "end of file"

%locations

%%

prog_option
   : input     { driver.build_input(element_list); }
   | END
   ;

input
   : function_statements         { element_list.push_back($1); }
   | input function_statements   { element_list.push_back($2); }
   | multiple_statements         { element_list.push_back($1); }
   | input multiple_statements   { element_list.push_back($2); }
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

conditional
   : LTE { $$ = static_cast<int>(NHLL::Conditionals::LTE); }
   | GTE { $$ = static_cast<int>(NHLL::Conditionals::GTE); }
   | GT  { $$ = static_cast<int>(NHLL::Conditionals::GT ); }
   | LT  { $$ = static_cast<int>(NHLL::Conditionals::LT ); }
   | EQ  { $$ = static_cast<int>(NHLL::Conditionals::EQ ); }
   | NE  { $$ = static_cast<int>(NHLL::Conditionals::NE ); }
   ;

condexpr
   : IDENTIFIER                          { $$ = new NHLL::ConditionalExpression(NHLL::ConditialExpressionType::ID   , Conditionals::NONE , "", ""); }
   | INTEGER_LITERAL                     { $$ = new NHLL::ConditionalExpression(NHLL::ConditialExpressionType::INT  , Conditionals::NONE , "", ""); }
   | REAL_LITERAL                        { $$ = new NHLL::ConditionalExpression(NHLL::ConditialExpressionType::REAL , Conditionals::NONE , "", ""); }
   | expression conditional expression   { $$ = new NHLL::ConditionalExpression(NHLL::ConditialExpressionType::EXPR , static_cast<NHLL::Conditionals>($2) , $1, $3 );              }
   ;

primary
    : IDENTIFIER                          { $$ = $1; }
    | INTEGER_LITERAL                     { $$ = $1; }
    | REAL_LITERAL                        { $$ = $1; }
    | '(' expression ')'                  { $$ = std::string( "(" + $2 + ")"); }
    ;

stmt
   : use_stmt     { $$ = $1; }
   | set_stmt     { $$ = $1; }
   | call_stmt    { $$ = $1; }
   | while_stmt   { $$ = $1; }
   ;

function_statements
   : function_stmt                     { $$ = std::vector<NhllElement*>(); $$.push_back($1); }
   | function_statements function_stmt { $1.push_back($2); $$ = $1; }
   ;

use_stmt
   : USE '(' STRING_LITERAL ')'                    { $$ = driver.create_use_statement($3);     }
   | USE '(' STRING_LITERAL ',' STRING_LITERAL ')' { $$ = driver.create_use_statement($3, $5); }
   ;

set_stmt
   : SET '(' identifiers ',' expression ')'         { $$ = driver.create_set_statement($3, $5); }
   ;

call_stmt
   : CALL '(' identifiers ')'                              { std::cout << "Empty  call"  << std::endl; $$ = nullptr; }
   | CALL '(' identifiers ',' '[' send_paramaters ']' ')'  { std::cout << "Filled call"  << std::endl; $$ = nullptr; }
   | PCALL '(' identifiers ')'                             { std::cout << "Empty  pcall" << std::endl; $$ = nullptr; }
   | PCALL '(' identifiers ',' '[' send_paramaters ']' ')' { std::cout << "Filled pcall" << std::endl; $$ = nullptr; }
   ;

while_stmt
   :  WHILE '(' condexpr ',' block ')'              { $$ = driver.create_while_statement($3, $5); delete $3; }
   |  WHILE '(' condexpr ',' '{' '}' ')'            { $$ = driver.create_while_statement($3, ElementList()); }
   ;

multiple_statements 
   : stmt                        { $$ = std::vector<NhllElement*>(); $$.push_back($1); }     // Create the list to return, and add the statement
   | multiple_statements stmt    { $1.push_back($2); $$ = $1; }                              // Add the statement to the list 
   ;

block 
   : '{' multiple_statements '}' { $$ = $2; }   // Return the statement list
   ;

recv_paramaters 
   : IDENTIFIER                                    { $$ = std::vector<std::string>(); $$.push_back($1); }
   | recv_paramaters ',' IDENTIFIER                { $$.push_back($3); }
   ;

send_paramaters 
   : identifiers                                    { $$ = std::vector<std::string>(); $$.push_back($1); }
   | send_paramaters ',' identifiers                { $$.push_back($3); }
   ;

function_stmt
   : FUNC_DECL '(' IDENTIFIER ',' block ')'                             { $$ = driver.create_function_statement($3, std::vector<std::string>(), $5); }
   | FUNC_DECL '(' IDENTIFIER ',' '[' recv_paramaters ']' ',' block ')' { $$ = driver.create_function_statement($3, $6, $9); }
   ;


%%

void NHLL::NHLL_Parser::error( const location_type &l, const std::string &err_message )
{
   std::cerr << "Error: " << err_message << " at " << l << "\n";
}
