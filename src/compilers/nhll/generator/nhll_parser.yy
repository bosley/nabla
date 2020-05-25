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

   std::vector<std::string> recv_params;


   std::vector<std::shared_ptr<NHLL::NhllElement>>  blockStatements;

   int statement_counter(0);

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

%type<int> block;
%type<int> multiple_statements;


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
   : input
   | END
   ;

input
   : function_stmt               {  }
   | input function_stmt         {  }
   | multiple_statements         { driver.global_statements(blockStatements); blockStatements.clear(); statement_counter = 0; }
   | input multiple_statements   {  }
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
   : use_stmt     
   | set_stmt     
   | call_stmt    
   | while_stmt
   ;

function_stmt
   : function_stmt function_stmt
   | function_decl         { }
   ;

use_stmt
   : USE '(' STRING_LITERAL ')'                    {  blockStatements.push_back(driver.create_use_statement($3));     }
   | USE '(' STRING_LITERAL ',' STRING_LITERAL ')' {  blockStatements.push_back(driver.create_use_statement($3, $5)); }
   ;

set_stmt
   : SET '(' identifiers ',' expression ')'         { blockStatements.push_back(driver.create_set_statement($3, $5)); }
   ;

call_stmt
   : CALL '(' identifiers ')'                              { std::cout << "Empty  call"  << std::endl; }
   | CALL '(' identifiers ',' '[' send_paramaters ']' ')'  { std::cout << "Filled call"  << std::endl; }
   | PCALL '(' identifiers ')'                             { std::cout << "Empty  pcall" << std::endl; }
   | PCALL '(' identifiers ',' '[' send_paramaters ']' ')' { std::cout << "Filled pcall" << std::endl; }
   ;

while_stmt
   :  WHILE '(' condexpr ',' block ')'              { 
                                                         // Build the statements belonging to the while statement from the block vector
                                                         std::vector<std::shared_ptr<NHLL::NhllElement>> while_elements(
                                                            blockStatements.begin()+(blockStatements.size()-$5+1), 
                                                            blockStatements.end()
                                                         );

                                                         // Reform the block statement
                                                         blockStatements = std::vector<std::shared_ptr<NHLL::NhllElement>>(
                                                            blockStatements.begin(), blockStatements.begin()+(blockStatements.size()-$5+1)
                                                         );

                                                         // Add the while statement to the statement vector
                                                         blockStatements.push_back(driver.create_while_statement($3, while_elements));
                                                         
                                                         delete $3;
                                                    }
   |  WHILE '(' condexpr ',' '{' '}' ')'            { driver.statement_while($3); delete $3; }
   ;

multiple_statements 
   : stmt                        { $$ = ++statement_counter; }
   | multiple_statements stmt    { $$ = ++statement_counter; }
   ;

block 
   : '{' multiple_statements '}' { $$ = $2; statement_counter = 0;}
   ;

recv_paramaters 
   : IDENTIFIER                                    { recv_params.push_back($1); }
   | IDENTIFIER ',' recv_paramaters                { recv_params.push_back($1); }
   ;

send_paramaters 
   : identifiers                                    { /* std::cout << "ID : " << $1 << std::endl; */}
   | identifiers ',' send_paramaters                { /* std::cout << "ID : " << $1 << std::endl; */}
   ;

function_decl
   : FUNC_DECL '(' IDENTIFIER ',' block ')'                             { driver.function_decl($3, recv_params, blockStatements); recv_params.clear(); blockStatements.clear(); }
   | FUNC_DECL '(' IDENTIFIER ',' '[' recv_paramaters ']' ',' block ')' { driver.function_decl($3, recv_params, blockStatements); recv_params.clear(); blockStatements.clear(); }
   ;


%%

void NHLL::NHLL_Parser::error( const location_type &l, const std::string &err_message )
{
   std::cerr << "Error: " << err_message << " at " << l << "\n";
}
