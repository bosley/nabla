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
         std::cerr << "Failed to allocate scanner: (" << ba.what() << ")\n";
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
         std::cerr << "Failed to allocate parser: (" <<  ba.what() << ")\n";
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

   bool NHLL_Driver::indicate_complete()
   {
      return code_generator.finalize();
   }

   // ----------------------------------------------------------
   //
   // ----------------------------------------------------------
   
   void  NHLL_Driver::build_element(NHLL::NhllElement* element)
   {
      if(element)
      {
         element->visit(*this);
         delete element;
      }
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
   
   void NHLL_Driver::accept(AsmStmt &stmt)
   {
      if(!code_generator.asm_block(stmt.asm_block))
      {
         exit(EXIT_FAILURE);
      }
   }

   // ----------------------------------------------------------
   //
   // ----------------------------------------------------------
   
   void NHLL_Driver::accept(DeclInteger &stmt)
   {
      if(!code_generator.declare_integer(stmt.identifier, stmt.set_to))
      {
         exit(EXIT_FAILURE);
      }
   }

   // ----------------------------------------------------------
   //
   // ----------------------------------------------------------
   
   void NHLL_Driver::accept(DeclReal &stmt)
   {
      if(!code_generator.declare_real(stmt.identifier, stmt.set_to))
      {
         exit(EXIT_FAILURE);
      }
   }

   // ----------------------------------------------------------
   //
   // ----------------------------------------------------------
   
   void NHLL_Driver::accept(DeclString &stmt)
   {
      if(!code_generator.declare_string(stmt.identifier, stmt.set_to, stmt.size))
      {
         exit(EXIT_FAILURE);
      }
   }

   // ----------------------------------------------------------
   //
   // ----------------------------------------------------------
   
   void NHLL_Driver::accept(ReAssignStmt &stmt)
   {
      if(!code_generator.reassign_variable(stmt.identifier, stmt.set_to, stmt.is_expr))
      {
         exit(EXIT_FAILURE);
      }
   }

   // ----------------------------------------------------------
   //
   // ----------------------------------------------------------
   
   void NHLL_Driver::accept(GlobalStmt &stmt)
   {
      if(!code_generator.global_variable(stmt.identifier, stmt.set_to, stmt.is_expr))
      {
         exit(EXIT_FAILURE);
      }
   }

   // ----------------------------------------------------------
   //
   // ----------------------------------------------------------
   
   void NHLL_Driver::accept(WhileStmt &stmt)
   {
      if(!code_generator.start_while(stmt.condition)) { exit(EXIT_FAILURE); }

      for(auto & inner_stmt : stmt.elements)
      {
         if(inner_stmt)
         {
            inner_stmt->visit(*this);
         }
      }

      if(!code_generator.end_while()) { exit(EXIT_FAILURE); }
   }
   
   // ----------------------------------------------------------
   //
   // ----------------------------------------------------------
   
   void NHLL_Driver::accept(LoopStmt &stmt)
   {
      if(!code_generator.start_loop(stmt.id)) { exit(EXIT_FAILURE); }

      for(auto & inner_stmt : stmt.elements)
      {
         if(inner_stmt)
         {
            inner_stmt->visit(*this);
         }
      }

      if(!code_generator.end_loop()) { exit(EXIT_FAILURE); }
   }
   
   // ----------------------------------------------------------
   //
   // ----------------------------------------------------------
   
   void NHLL_Driver::accept(BreakStmt &stmt)
   {
      if(!code_generator.break_loop(stmt.id)) { exit(EXIT_FAILURE); }
   }

   // ----------------------------------------------------------
   //
   // ----------------------------------------------------------
   
   void NHLL_Driver::accept(CallStmt &stmt)
   {
      if(!code_generator.call_method(stmt.function, stmt.params)) { exit(EXIT_FAILURE); }
   }

   // ----------------------------------------------------------
   //
   // ----------------------------------------------------------
   
   void NHLL_Driver::accept(NhllFunction &stmt)
   {
      if(!code_generator.start_function(stmt.name, stmt.params, stmt.return_type))
      {
         exit(EXIT_FAILURE);
      }

      for(auto & inner_stmt : stmt.elements)
      {
         if(inner_stmt)
         {
            inner_stmt->visit(*this);
         }
      }

      if(!code_generator.end_function())
      {
         exit(EXIT_FAILURE);
      }
   }

   // ----------------------------------------------------------
   //
   // ----------------------------------------------------------
   
   void NHLL_Driver::accept(LeaveStmt &stmt)
   {
      switch(stmt.variant)
      {
         case LeaveStmt::Variant::YIELD:
         {
            if(!code_generator.yield_statement(stmt.value, stmt.is_expr)){ exit(EXIT_FAILURE); }
            break;
         }

         case LeaveStmt::Variant::RETURN:
         {
            if(!code_generator.return_statement(stmt.value, stmt.is_expr)){ exit(EXIT_FAILURE); }
            break;
         }

         default:
            std::cerr << "Driver::Error : A leave statement has an unhandled variant!" << std::endl;
            std::cerr << "If you're seeing this message mark it on you calendar as a day you saw "
                      << "actual black magic happen." << std::endl;
            exit(EXIT_FAILURE);
         break;
      }
   }

   // ----------------------------------------------------------
   //
   // ----------------------------------------------------------
   
   void NHLL_Driver::accept(CheckCondition &stmt)
   {
      if(!code_generator.start_check_condition(stmt.condition)) { exit(EXIT_FAILURE); }

      for(auto & inner_stmt : stmt.elements)
      {
         if(inner_stmt)
         {
            inner_stmt->visit(*this);
         }
      }

      if(!code_generator.end_check_condition()) { exit(EXIT_FAILURE); }
   
   }
   
   // ----------------------------------------------------------
   //
   // ----------------------------------------------------------
   
   void NHLL_Driver::accept(CheckStmt &stmt)
   {
      if(!code_generator.start_check()) { exit(EXIT_FAILURE); }

      for(auto & inner_stmt : stmt.elements)
      {
         if(inner_stmt)
         {
            inner_stmt->visit(*this);
         }
      }

      if(!code_generator.end_check()) { exit(EXIT_FAILURE); }
   }

   // ----------------------------------------------------------
   //
   // ----------------------------------------------------------
   
   void NHLL_Driver::accept(ExitStmt &stmt)
   {
      if(!code_generator.exit_statement()) { exit(EXIT_FAILURE); }
   }
}