#include <cctype>
#include <fstream>
#include <cassert>
#include <iostream>
#include <string>
#include "del_driver.hpp"

namespace DEL
{
   DEL_Driver::DEL_Driver() : 
                              error_man(tracker), 
                              symbol_table(error_man, memory_man),
                              code_gen(error_man, symbol_table, memory_man),
                              analyzer(error_man, symbol_table, code_gen, memory_man)
   {
      symbol_table.new_context("global");
   }

   // ----------------------------------------------------------
   //
   // ----------------------------------------------------------

   DEL_Driver::~DEL_Driver()
   {
      delete(scanner);
      scanner = nullptr;
      delete(parser);
      parser = nullptr;
   }

   // ----------------------------------------------------------
   //
   // ----------------------------------------------------------
   
   void DEL_Driver::parse( const char * const filename )
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
   
   void DEL_Driver::parse( std::istream &stream )
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
   
   void  DEL_Driver::parse_helper( std::istream &stream )
   {
      delete(scanner);
      try
      {
         scanner = new DEL::DEL_Scanner( &stream, (*this) );
      }
      catch( std::bad_alloc &ba )
      {
         std::cerr << "Failed to allocate scanner: (" << ba.what() << ")\n";
         exit( EXIT_FAILURE );
      }

      delete(parser); 
      try
      {
         parser = new DEL::DEL_Parser( (*scanner) /* scanner */, 
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
   
   std::ostream& DEL_Driver::print( std::ostream &stream )
   {
      //stream << "done" << "\n";
      return(stream);
   }

   // ----------------------------------------------------------
   //
   // ----------------------------------------------------------

   void DEL_Driver::inc_line()
   {
      tracker.inc_lines_tracked();
   }

   void DEL_Driver::indicate_complete()
   {
      std::cout << "Complete!" << std::endl;
   }

   // ----------------------------------------------------------
   //
   // ----------------------------------------------------------

   void DEL_Driver::build_function(Function *function)
   {
      // Trigger the analyzer with a function
      analyzer.build_function(function);
   }
}