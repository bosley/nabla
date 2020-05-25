#include <cctype>
#include <fstream>
#include <cassert>
#include <iostream>
#include "nhll_driver.hpp"

namespace NHLL
{
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
         scanner = new NHLL::NHLL_Scanner( &stream );
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

   void NHLL_Driver::end_of_statement()
   {
      std::cout << "End of statement" << std::endl;

      for( auto &i : currentElements)
      {
         if(i)
         {
            i->visit(*this);
         }
      }
      // Statements that exist outside of functions need to be popped and generated
      currentElements.clear();
   }

   // ----------------------------------------------------------
   //
   // ----------------------------------------------------------

   void NHLL_Driver::function_decl(std::string name, std::vector<std::string> params)
   {
      std::cout << "driver.function_decl(" << name << ", " << "[";
      for(auto &i : params)
      {
         std::cout << " " << i ;
      }
      std::cout << "])" << std::endl;

      end_of_statement();
   }

   // ----------------------------------------------------------
   //
   // ----------------------------------------------------------
   
   void NHLL_Driver::statement_set(std::string lhs, std::string expression)
   {
      SetStmt ss(lhs, expression);  // Convert to post fix here


      std::shared_ptr<NHLL::NhllElement> s = std::make_shared<NHLL::SetStmt>(&ss);

      currentElements.push_back( std::move(s) );

      //std::cout << "driver.statement_set(" << lhs << ", " << expression << ")" << std::endl;
   }

   // ----------------------------------------------------------
   //
   // ----------------------------------------------------------
   
   void NHLL_Driver::statement_use(std::string lhs, std::string rhs)
   {
      if(rhs.size() == 0)
      {
         rhs = lhs;
      }

      UseStmt us(lhs, rhs);
      
      std::shared_ptr<NHLL::NhllElement> s = std::make_shared<NHLL::UseStmt>(&us);

      currentElements.push_back( std::move(s) );

      //std::cout << "driver.statement_use(" << lhs << ", " << rhs << ")" << std::endl;
   }

   // ------------------------------------------------------------------------

   void NHLL_Driver::accept(UseStmt &stmt)
   {
      std::cout << "Generate use statement ! " << stmt.module << ", " << stmt.as_name << std::endl;
   }
   
   void NHLL_Driver::accept(SetStmt &stmt)
   {
      std::cout << "Generate set statement ! " << stmt.identifier << ", " << stmt.expression << std::endl;
   }
}