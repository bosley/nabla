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
#include "nhll_postfix.hpp"

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

      //! \brief Call visit on all elements to generate code
      void build_input(std::vector< ElementList > input_elements );

      NHLL::NhllElement* create_function_statement(std::string name, std::vector<FunctionParam> params, DataPrims ret, ElementList elements);
      NHLL::NhllElement* create_let_statement(std::string lhs, std::string rhs, bool is_expression=true);
      NHLL::NhllElement* create_reassign_statement(std::string lhs, std::string rhs, bool is_expression=true);
      NHLL::NhllElement* create_use_statement(std::string lhs, std::string rhs = "");
      NHLL::NhllElement* create_while_statement(ConditionalExpression *expr, ElementList elements);
      NHLL::NhllElement* create_loop_statement(std::string id, ElementList elements);
      NHLL::NhllElement* create_break_statement(std::string id);
      NHLL::NhllElement* create_call_statement(std::string function, std::vector<std::string> params);

      //! \brief Visit a use statement, triggers code generation
      virtual void accept(UseStmt &stmt) override;

      //! \brief Visit a set statement, triggers code generation
      virtual void accept(LetStmt &stmt) override;

      //! \brief Visit a set statement, triggers code generation
      virtual void accept(ReAssignStmt &stmt) override;

      //! \brief Visit a while statement, triggers code generation
      virtual void accept(WhileStmt &stmt) override;

      //! \brief Visit a while statement, triggers code generation
      virtual void accept(LoopStmt &stmt) override;

      //! \brief Visit a while statement, triggers code generation
      virtual void accept(BreakStmt &stmt) override;

      //! \brief Visit a while statement, triggers code generation
      virtual void accept(CallStmt &stmt) override;

      //! \brief Visit a while statement, triggers code generation
      virtual void accept(NhllFunction &stmt) override;

   private:

      void parse_helper( std::istream &stream );

      NHLL::NHLL_Parser  *parser  = nullptr;
      NHLL::NHLL_Scanner *scanner = nullptr;

      Postfix postfixer;
   };

}
#endif
