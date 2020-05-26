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
      //struct FunctionParam;
      //enum class DataPrims;
   }

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
   #include <vector>
   
   #include "nhll.hpp"
   #include "nhll_driver.hpp"

   std::vector< std::vector<NHLL::NhllElement*> > element_list;
   std::vector< NHLL::FunctionParam > r_paramaters;
   std::vector< std::string > s_paramaters;

#undef yylex
#define yylex scanner.yylex
}

%define api.value.type variant
%define parse.assert

%type<std::string> expression;
%type<std::string> ope;
%type<std::string> primary;
%type<std::string> passable;
%type<std::string> identifiers;
%type<NHLL::ConditionalExpression*> condexpr;
%type<int> conditional;
%type<int> data_prim;

%type<NHLL::NhllElement*> stmt;
%type<NHLL::NhllElement*> use_stmt;
%type<NHLL::NhllElement*> let_stmt;
%type<NHLL::NhllElement*> reassign_stmt;
%type<NHLL::NhllElement*> call_stmt;
%type<NHLL::NhllElement*> while_stmt;
%type<NHLL::NhllElement*> loop_stmt;
%type<NHLL::NhllElement*> break_stmt;
%type<NHLL::NhllElement*> function_stmt;

%type<std::vector<NhllElement*>> multiple_statements;
%type<std::vector<NhllElement*>> block;
%type<std::vector<NhllElement*>> function_statements;

%token FUNC_DECL USE LET CALL PCALL WHILE LOOP AS INT NIL REAL STR RET_ARROW BREAK
%token LTE GTE LT GT EQ NE 

%token <std::string> STRING_LITERAL
%token <std::string> INTEGER_LITERAL
%token <std::string> REAL_LITERAL
%token <std::string> IDENTIFIER


%token               END    0     "end of file"

%locations

%%

prog_option
   : input     { driver.build_input(element_list); element_list.clear(); } // The driver deletes all allocated space for elements
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

data_prim
   :  INT  { $$ = static_cast<int>(NHLL::DataPrims::INT ); }
   |  REAL { $$ = static_cast<int>(NHLL::DataPrims::REAL); }
   |  STR  { $$ = static_cast<int>(NHLL::DataPrims::STR ); }
   |  NIL  { $$ = static_cast<int>(NHLL::DataPrims::NIL ); }
   ;

recv_paramaters 
   : data_prim IDENTIFIER                       { r_paramaters.push_back(FunctionParam { static_cast<DataPrims>($1), $2 }); }
   | recv_paramaters ',' data_prim IDENTIFIER   { r_paramaters.push_back(FunctionParam { static_cast<DataPrims>($3), $4 }); }
   ;

send_paramaters 
   : passable                                   { s_paramaters.push_back($1); }
   | send_paramaters ',' passable               { s_paramaters.push_back($3); }
   ;

primary
    : INTEGER_LITERAL                     { $$ = $1; }
    | REAL_LITERAL                        { $$ = $1; }
    | identifiers                         { $$ = $1; }
    | '(' expression ')'                  { $$ = std::string( "(" + $2 + ")"); }
    ;

passable
   : identifiers     { $$ = $1; }
   | INTEGER_LITERAL { $$ = $1; }
   | REAL_LITERAL    { $$ = $1; }
   | STRING_LITERAL  { $$ = $1; }
   ;

stmt
   : use_stmt      { $$ = $1; }
   | let_stmt      { $$ = $1; }
   | reassign_stmt { $$ = $1; }
   | call_stmt     { $$ = $1; }
   | while_stmt    { $$ = $1; }
   | loop_stmt     { $$ = $1; }
   | break_stmt    { $$ = $1; }
   ;

multiple_statements 
   : stmt                        { $$ = std::vector<NhllElement*>(); $$.push_back($1); }     // Create the list to return, and add the statement
   | multiple_statements stmt    { $1.push_back($2); $$ = $1; }                              // Add the statement to the list 
   ;

block 
   : '{' multiple_statements '}' { $$ = $2; }   // Return the statement list
   | '{' '}'                     { $$ = std::vector<NhllElement*>(); }
   ;

function_statements
   : function_stmt                        { $$ = std::vector<NhllElement*>(); $$.push_back($1); }
   | function_statements function_stmt    { $1.push_back($2); $$ = $1; }
   ;

use_stmt
   : USE identifiers                      { $$ = driver.create_use_statement($2);      }
   | USE identifiers AS IDENTIFIER        { $$ = driver.create_use_statement($2, $4);  }
   ;

let_stmt
   : LET identifiers '=' expression       { $$ = driver.create_let_statement($2, $4);        }
   | LET identifiers '=' STRING_LITERAL   { $$ = driver.create_let_statement($2, $4, false); }
   ;

reassign_stmt
   : identifiers '=' expression           { $$ = driver.create_reassign_statement($1, $3);        }
   | identifiers '=' STRING_LITERAL       { $$ = driver.create_reassign_statement($1, $3, false); }
   ;

call_stmt
   : identifiers '(' ')'                  { $$ = driver.create_call_statement($1, std::vector<std::string>()); }
   | identifiers '(' send_paramaters ')'  { $$ = driver.create_call_statement($1, s_paramaters); s_paramaters.clear(); }
   ;

while_stmt
   :  WHILE '(' condexpr ')' block        { $$ = driver.create_while_statement($3, $5); delete $3; }
   ;

loop_stmt
   : LOOP '.' IDENTIFIER block            { $$ = driver.create_loop_statement($3, $4); }
   ;

break_stmt
   : BREAK IDENTIFIER                     { $$ = driver.create_break_statement($2); }
   ;

function_stmt
   : FUNC_DECL IDENTIFIER '(' ')' RET_ARROW data_prim block                 { $$ = driver.create_function_statement($2, std::vector<FunctionParam>(), static_cast<DataPrims>($6), $7); }
   | FUNC_DECL IDENTIFIER '(' recv_paramaters ')' RET_ARROW data_prim block { $$ = driver.create_function_statement($2, r_paramaters, static_cast<DataPrims>($7), $8); r_paramaters.clear(); }
   ;
   

%%

void NHLL::NHLL_Parser::error( const location_type &l, const std::string &err_message )
{
   std::cerr << "Error: " << err_message << " at " << l << "\n";
}
