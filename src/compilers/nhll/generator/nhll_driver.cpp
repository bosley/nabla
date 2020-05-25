#include <cctype>
#include <fstream>
#include <cassert>
#include <iostream>
#include "nhll_driver.hpp"

/*
   stage_sections stack hold onto items that need to be generated relative to the section
   that they are defined in.

      <defined elements>         // Top - The current section ( if scope or something )
  __________________________
   |  <defined elements> |       while scope
   |  <defined elements> |       function scope
   |  <defined elements> |       global scope


*/

namespace NHLL
{
   NHLL_Driver::NHLL_Driver() : se_idx(0)
   {
      mark_subsection();
      stage_sections.reserve(100);
   }

   // ----------------------------------------------------------
   //
   // ----------------------------------------------------------

   NHLL_Driver::~NHLL_Driver()
   {
      for(auto & i : stage_sections)
      {
         i.clear();
      }

      stage_sections.clear();

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

   void NHLL_Driver::mark_subsection()
   {
      std::cout << "--- MARK! " << stage_sections.size() << std::endl;

      std::vector<std::shared_ptr<NHLL::NhllElement>> item;
      stage_sections.push_back(item);
      se_idx++;
   }

   void NHLL_Driver::unmark_subsection()
   {
      std::cout << "--- UNMARK! " << stage_sections.size() << std::endl;


      if(se_idx > 0)
      {
         stage_sections.pop_back();
         se_idx--;
      }
   }

   // ----------------------------------------------------------
   //
   // ----------------------------------------------------------

   void NHLL_Driver::end_parse()
   {
     // std::cout << "GLOBAL SECTION" << std::endl;
//
     // // Unmark global section
     // end_of_statement();
     // unmark_subsection();
   }

   // ----------------------------------------------------------
   //
   //    At the end of a statement, we visit all elements and
   //    trigger code generation specific to that element
   //
   // ----------------------------------------------------------

   void NHLL_Driver::end_of_statement()
   {
      for( auto &i : stage_sections[se_idx])
      {
         if(i)
         {
            i->visit(*this);
         }
      }
      //stage_sections[se_idx].clear();
   }


   void NHLL_Driver::global_statements(std::vector<std::shared_ptr<NHLL::NhllElement>> elements)
   {
      std::cout << "GLOBAL STATEMENTS" << std::endl;
      for( auto &i : elements)
      {
         if(i)
         {
            i->visit(*this);
         }
      }
   }

   // ----------------------------------------------------------
   //
   // ----------------------------------------------------------

   void NHLL_Driver::function_decl(std::string name, std::vector<std::string> params,  std::vector<std::shared_ptr<NHLL::NhllElement>> elements)
   {
      std::cout << "\n\ndriver.function_decl(" << name << ", " << "[";
      for(auto &i : params)
      {
         std::cout << " " << i ;
      }
      std::cout << "])" << std::endl;
 
      for( auto &i : elements)
      {
         if(i)
         {
            i->visit(*this);
         }
      }

   }

   std::shared_ptr<NHLL::NhllElement> NHLL_Driver::create_use_statement(std::string lhs, std::string rhs)
   {
      if(rhs.size() == 0)
      {
         rhs = lhs;
      }

      UseStmt us(lhs, rhs);
      return std::make_shared<NHLL::UseStmt>(&us);
   }



   std::shared_ptr<NHLL::NhllElement> NHLL_Driver::create_set_statement(std::string lhs, std::string rhs)
   {
      SetStmt ss(lhs, rhs);  // Convert to post fix here
      return std::make_shared<NHLL::SetStmt>(&ss);
   }


   std::shared_ptr<NHLL::NhllElement> NHLL_Driver::create_while_statement(ConditionalExpression *expr, std::vector<std::shared_ptr<NHLL::NhllElement>> elements)
   {
      WhileStmt ws(expr, elements);
      
      return std::make_shared<NHLL::WhileStmt>(&ws);
   }

   // ----------------------------------------------------------
   //
   // ----------------------------------------------------------
   
   void NHLL_Driver::statement_set(std::string lhs, std::string expression)
   {
      SetStmt ss(lhs, expression);  // Convert to post fix here

      std::shared_ptr<NHLL::NhllElement> s = std::make_shared<NHLL::SetStmt>(&ss);

      stage_sections[se_idx].push_back( std::move(s) );
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

      stage_sections[se_idx].push_back( std::move(s) );
   }

   // ----------------------------------------------------------
   //
   // ----------------------------------------------------------
   
   void NHLL_Driver::statement_while(ConditionalExpression *expr)
   {
      
     // WhileStmt ws(expr);
     // 
     // std::shared_ptr<NHLL::NhllElement> s = std::make_shared<NHLL::WhileStmt>(&ws);
//
     // stage_sections[se_idx].push_back( std::move(s) );
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
      std::cout << "Generate while statement ! " << std::endl; //<< stmt.identifier << ", " << stmt.expression << std::endl;

      std::cout << "\tSTACK DEPTH "  << stage_sections.size() << std::endl;
      // TODO HERE : Tell the bye generator that the next incomming data is for a while loop
      //             with the given condition
      
      // Go through each statement in the while loop, and generate the data
      for( auto &i : stmt.elements)
      {
         if(i)
         {
            i->visit(*this);
         }
      }

      // Unmark this while loop's subsection
      unmark_subsection();
   }
}