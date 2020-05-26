#include <cctype>
#include <fstream>
#include <cassert>
#include <iostream>
#include "nhll_driver.hpp"

namespace NHLL
{
   NHLL_Driver::NHLL_Driver() 
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
            }
         }
      }
   }

   // ----------------------------------------------------------
   //
   // ----------------------------------------------------------

   NHLL::NhllElement* NHLL_Driver::create_function_statement(std::string name, std::vector<std::string> params, ElementList elements)
   {
      return new NhllFunction(name, params, elements);
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

   NHLL::NhllElement* NHLL_Driver::create_set_statement(std::string lhs, std::string rhs)
   {
      return new SetStmt(lhs, rhs); // Convert to post fix here
   }

   // ----------------------------------------------------------
   //
   // ----------------------------------------------------------

   NHLL::NhllElement* NHLL_Driver::create_while_statement(ConditionalExpression *expr, ElementList elements)
   {
      std::cout << "Create while statement! " << std::endl;

      return new WhileStmt(expr, elements);
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
      std::cout << "Generate use statement ! " << stmt.module << ", " << stmt.as_name << std::endl;
   }
   
   // ----------------------------------------------------------
   //
   // ----------------------------------------------------------
   
   void NHLL_Driver::accept(SetStmt &stmt)
   {
      std::cout << "Generate set statement ! " << stmt.identifier << ", " << stmt.expression << std::endl;
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
   
   void NHLL_Driver::accept(NhllFunction &stmt)
   {
      std::cout << "Generate function ! " << stmt.name << std::endl;

      for(auto & inner_stmt : stmt.elements)
      {
         if(inner_stmt)
         {
            inner_stmt->visit(*this);
         }
      }
   }
}