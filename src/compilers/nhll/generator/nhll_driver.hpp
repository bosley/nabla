#ifndef __NHLLDRIVER_HPP__
#define __NHLLDRIVER_HPP__ 1

#include <string>
#include <cstddef>
#include <istream>
#include <vector>
#include <memory>

#include "nhll.hpp"
#include "nhll_scanner.hpp"
#include "nhll_parser.tab.hh"

namespace NHLL
{
   class NHLL_Driver : public NhllVisitor
   {
   public:
      NHLL_Driver() = default;
      virtual ~NHLL_Driver();
      
      //! \brief Parse from a file
      void parse( const char * const filename );

      //! \brief Parse from an istream
      void parse( std::istream &iss );

      //! \brief Print driver
      std::ostream& print(std::ostream &stream);

      //! \brief Mark the end of a 'statement' indicating that we need to gen some code
      void end_of_statement();

      //! \brief Function declaration. 
      //! \param name The name of the function
      //! \param params The parameters of the function
      //! \post Marks end_of_statement 
      void function_decl(std::string name, std::vector<std::string> params);

      //! \brief Mark the presence of a 'set' statement
      //! \param lhs Identifier 
      //! \param expression The expression to set the statement to
      void statement_set(std::string lhs, std::string expression);

      //! \brief Mark the presence of a 'use' statement
      //! \param lhs The module to use
      //! \param rhs The name to use the module as, defaults to lhs
      void statement_use(std::string lhs, std::string rhs = "");


      //! \brief Visit a use statement, triggers code generation
      virtual void accept(UseStmt &stmt) override;

      //! \brief Visit a set statement, triggers code generation
      virtual void accept(SetStmt &stmt) override;

   private:

      void parse_helper( std::istream &stream );

      NHLL::NHLL_Parser  *parser  = nullptr;
      NHLL::NHLL_Scanner *scanner = nullptr;

      std::vector<std::shared_ptr<NHLL::NhllElement>>  currentElements;

   };

}
#endif
