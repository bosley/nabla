#ifndef __NHLLDRIVER_HPP__
#define __NHLLDRIVER_HPP__ 1

#include <string>
#include <cstddef>
#include <istream>
#include <vector>
#include <memory>
#include <stack>

#include "nhll.hpp"
#include "nhll_scanner.hpp"
#include "nhll_parser.tab.hh"

namespace NHLL
{
   class NHLL_Driver : public NhllVisitor
   {
   public:
      NHLL_Driver();

      virtual ~NHLL_Driver();
      
      //! \brief Parse from a file
      void parse( const char * const filename );

      //! \brief Parse from an istream
      void parse( std::istream &iss );

      //! \brief Print driver
      std::ostream& print(std::ostream &stream);

      void end_parse();

      void mark_subsection();

      void unmark_subsection();

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

      void statement_while(ConditionalExpression *expr);

      //! \brief Visit a use statement, triggers code generation
      virtual void accept(UseStmt &stmt) override;

      //! \brief Visit a set statement, triggers code generation
      virtual void accept(SetStmt &stmt) override;

      //! \brief Visit a while statement, triggers code generation
      virtual void accept(WhileStmt &stmt) override;

   private:

      void parse_helper( std::istream &stream );

      NHLL::NHLL_Parser  *parser  = nullptr;
      NHLL::NHLL_Scanner *scanner = nullptr;

      // The parts of the language that have been detected in their base element form
      // from these base elements we forge ahead with generating byte code
      std::vector< std::vector<std::shared_ptr<NHLL::NhllElement>> > stage_sections;
      uint64_t se_idx;
   };

}
#endif
