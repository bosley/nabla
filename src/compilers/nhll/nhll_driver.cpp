#include <cctype>
#include <fstream>
#include <cassert>
#include <iostream>
#include "CodeGen.hpp"
#include "nhll_driver.hpp"

namespace NHLL
{
   NHLL_Driver::NHLL_Driver(NHLL::CodeGen &code_generator) : code_generator(code_generator) 
   {
      
   }

   // ----------------------------------------------------------
   //
   // ----------------------------------------------------------

   NHLL_Driver::~NHLL_Driver()
   {
      delete(scanner);
      scanner = nullptr;
      delete(parser);
      parser = nullptr;
   }

   // ----------------------------------------------------------
   //
   // ----------------------------------------------------------
   
   void NHLL_Driver::parse( const char * const filename )
   {
      assert( filename != nullptr );
      std::ifstream in_file( filename );
      if( ! in_file.good() )
      {
         exit( EXIT_FAILURE );
      }
      parse_helper( in_file );
      return;
   }

   // ----------------------------------------------------------
   //
   // ----------------------------------------------------------
   
   void NHLL_Driver::parse( std::istream &stream )
   {
      if( ! stream.good()  && stream.eof() )
      {
         return;
      }
      //else
      parse_helper( stream ); 
      return;
   }

   // ----------------------------------------------------------
   //
   // ----------------------------------------------------------
   
   void  NHLL_Driver::parse_helper( std::istream &stream )
   {
      delete(scanner);
      try
      {
         scanner = new NHLL::NHLL_Scanner( &stream, (*this) );
      }
      catch( std::bad_alloc &ba )
      {
         std::cerr << "Failed to allocate scanner: (" <<
            ba.what() << "), exiting!!\n";
         exit( EXIT_FAILURE );
      }

      delete(parser); 
      try
      {
         parser = new NHLL::NHLL_Parser( (*scanner) /* scanner */, 
                                       (*this) /* driver */ );
      }
      catch( std::bad_alloc &ba )
      {
         std::cerr << "Failed to allocate parser: (" << 
            ba.what() << "), exiting!!\n";
         exit( EXIT_FAILURE );
      }
      const int accept( 0 );
      if( parser->parse() != accept )
      {
         std::cerr << "Parse failed!!\n";
      }
      return;
   }

   // ----------------------------------------------------------
   //
   // ----------------------------------------------------------
   
   std::ostream& NHLL_Driver::print( std::ostream &stream )
   {
      stream << "done" << "\n";
      return(stream);
   }

   // ----------------------------------------------------------
   //
   // ----------------------------------------------------------

   void NHLL_Driver::build_input(std::vector< ElementList > input_elements )
   {
      std::cout << "Build from : " << input_elements.size() << " lists of elements" << std::endl;

      for(auto &element_list : input_elements )
      {
         std::cout << "----- Current element list has " << element_list.size() << " elements" << std::endl;

         for(auto &element : element_list)
         {
            if(element)
            {
               element->visit(*this);
               delete element;
            }
         }
         element_list.clear();
      }
      input_elements.clear();
   }

   // ----------------------------------------------------------
   //
   // ----------------------------------------------------------

   NHLL::NhllElement* NHLL_Driver::create_function_statement(std::string name, std::vector<FunctionParam> params, DataPrims ret, ElementList elements)
   {
      return new NhllFunction(name, params, ret, elements);
   }

   // ----------------------------------------------------------
   //
   // ----------------------------------------------------------

   NHLL::NhllElement* NHLL_Driver::create_use_statement(std::string lhs, std::string rhs)
   {
      if(rhs.size() == 0)
      {
         rhs = lhs;
      }
      return new UseStmt(lhs, rhs);
   }

   // ----------------------------------------------------------
   //
   // ----------------------------------------------------------

   NHLL::NhllElement* NHLL_Driver::create_let_statement(std::string lhs, std::string rhs, bool is_expression)
   {
      return new LetStmt(lhs, rhs, is_expression);
   }

   // ----------------------------------------------------------
   //
   // ----------------------------------------------------------

   NHLL::NhllElement* NHLL_Driver::create_reassign_statement(std::string lhs, std::string rhs, bool is_expression)
   {
      return new ReAssignStmt(lhs, rhs, is_expression);
   }

   // ----------------------------------------------------------
   //
   // ----------------------------------------------------------

   NHLL::NhllElement* NHLL_Driver::create_while_statement(ConditionalExpression *expr, ElementList elements)
   {
      std::cout << "Create while statement! " << std::endl;

      return new WhileStmt(expr, elements);
   }

   // ----------------------------------------------------------
   //
   // ----------------------------------------------------------

   NHLL::NhllElement* NHLL_Driver::create_loop_statement(std::string id, ElementList elements)
   {
      std::cout << "Create loop statement! " << std::endl;

      return new LoopStmt(id, elements);
   }

   // ----------------------------------------------------------
   //
   // ----------------------------------------------------------

   NHLL::NhllElement* NHLL_Driver::create_break_statement(std::string id)
   {
      std::cout << "Create break statement! " << std::endl;

      return new BreakStmt(id);
   }

   // ----------------------------------------------------------
   //
   // ----------------------------------------------------------

   NHLL::NhllElement* NHLL_Driver::create_call_statement(std::string function, std::vector<std::string> params)
   {
      return new CallStmt(function, params);
   }

   // -----------------------------------------------------------------------------------------------------------
   //
   //       Accept functions from base visitor class that are used to call into the code generation
   //       functions
   //
   // -----------------------------------------------------------------------------------------------------------

   // ----------------------------------------------------------
   //
   // ----------------------------------------------------------
   
   void NHLL_Driver::accept(UseStmt &stmt)
   {
      code_generator.gen_use_statement(UseStmt(stmt));
   }
   
   // ----------------------------------------------------------
   //
   // ----------------------------------------------------------
   
   void NHLL_Driver::accept(LetStmt &stmt)
   {
      std::cout << "Generate let statement ! " << stmt.identifier << ", " << stmt.set_to << ", is expr : " << stmt.is_expr << std::endl;

      if(stmt.is_expr)
      {
         std::cout << "\tPostfix: ";
         for(auto & pfw : postfixer.convert(stmt.set_to))
         {
            std::cout << pfw.value << " ";
         }
         std::cout << std::endl;
      }
   }

   // ----------------------------------------------------------
   //
   // ----------------------------------------------------------
   
   void NHLL_Driver::accept(ReAssignStmt &stmt)
   {
      std::cout << "Generate reassign statement ! " << stmt.identifier << ", " << stmt.set_to << ", is expr : " << stmt.is_expr << std::endl;

      if(stmt.is_expr)
      {
         std::cout << "\tPostfix: ";
         for(auto & pfw : postfixer.convert(stmt.set_to))
         {
            std::cout << pfw.value << " ";
         }
         std::cout << std::endl;
      }
   }

   // ----------------------------------------------------------
   //
   // ----------------------------------------------------------
   
   void NHLL_Driver::accept(WhileStmt &stmt)
   {
      std::cout << "Generate while statement ! " << std::endl;
      
      for(auto & inner_stmt : stmt.elements)
      {
         if(inner_stmt)
         {
            inner_stmt->visit(*this);
         }
      }
   }
   
   // ----------------------------------------------------------
   //
   // ----------------------------------------------------------
   
   void NHLL_Driver::accept(LoopStmt &stmt)
   {
      std::cout << "Generate loop statement ! id : " << stmt.id << std::endl;
      
      for(auto & inner_stmt : stmt.elements)
      {
         if(inner_stmt)
         {
            inner_stmt->visit(*this);
         }
      }
   }
   
   // ----------------------------------------------------------
   //
   // ----------------------------------------------------------
   
   void NHLL_Driver::accept(BreakStmt &stmt)
   {
      std::cout << "Generate break statement ! " << stmt.id << std::endl;
   }

   // ----------------------------------------------------------
   //
   // ----------------------------------------------------------
   
   void NHLL_Driver::accept(CallStmt &stmt)
   {
      std::cout << "Generate call statement ! " << stmt.function << ", " << " params : (";

      for(auto &p : stmt.params)
      {
         std::cout << " " << p;
      }
      std::cout << " ) " << std::endl;
   }

   // ----------------------------------------------------------
   //
   // ----------------------------------------------------------
   
   void NHLL_Driver::accept(NhllFunction &stmt)
   {
      std::cout << "Generate function ! " << stmt.name << " params : (";

      for(auto &p : stmt.params)
      {
         std::cout << "type: " << DataPrims_to_string(p.type) << ", name: " << p.name;
      }
      std::cout << " ) returns : " << DataPrims_to_string(stmt.return_type) << std::endl;


      for(auto & inner_stmt : stmt.elements)
      {
         if(inner_stmt)
         {
            inner_stmt->visit(*this);
         }
      }
   }
}