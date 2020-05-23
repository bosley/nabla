#include <cctype>
#include <fstream>
#include <cassert>
#include <iostream>
#include "nhll_driver.hpp"

NHLL::NHLL_Driver::~NHLL_Driver()
{
   delete(scanner);
   scanner = nullptr;
   delete(parser);
   parser = nullptr;
}

void NHLL::NHLL_Driver::parse( const char * const filename )
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

void NHLL::NHLL_Driver::parse( std::istream &stream )
{
   if( ! stream.good()  && stream.eof() )
   {
       return;
   }
   //else
   parse_helper( stream ); 
   return;
}


void  NHLL::NHLL_Driver::parse_helper( std::istream &stream )
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

std::ostream& NHLL::NHLL_Driver::print( std::ostream &stream )
{
   stream << "done" << "\n";
   return(stream);
}
