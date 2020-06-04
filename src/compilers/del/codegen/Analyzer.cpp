#include "Analyzer.hpp"

#include <iostream>

namespace DEL
{
    // ----------------------------------------------------------
    //
    // ----------------------------------------------------------

    Analyzer::Analyzer(Errors & err, SymbolTable & symbolTable, Codegen & code_gen) : 
                                                                        error_man(err), 
                                                                        symbol_table(symbolTable),
                                                                        code_gen(code_gen)
    {

    }

    // ----------------------------------------------------------
    //
    // ----------------------------------------------------------

    Analyzer::~Analyzer()
    {
        
    }

    // ----------------------------------------------------------
    //
    // ----------------------------------------------------------

    void Analyzer::ensure_unique_symbol(std::string id)
    {
        if(symbol_table.does_symbol_exist(id, true))
        {
            error_man.report_previously_declared(id);
        }
    }

    // ----------------------------------------------------------
    //
    // ----------------------------------------------------------

    void Analyzer::ensure_id_in_current_context(std::string id, std::vector<ValType> allowed)
    {
        // Check symbol table to see if an id exists, don't display information yet
        if(!symbol_table.does_symbol_exist(id, false))
        {
            // Reports the error and true marks the program for death
            error_man.report_unknown_id(id, true);
        }

        // Ensure type is one of the allowed types
        bool is_allowed = false;
        for(auto & v : allowed)
        {
            if(symbol_table.is_existing_symbol_of_type(id, v))
            {
            is_allowed = true;
            }
        }

        // If the type isn't allowed
        if(!is_allowed)
        {
            error_man.report_unallowed_type(id, true);
        }
    }

    // ----------------------------------------------------------
    //
    // ----------------------------------------------------------

    ValType Analyzer::get_id_type(std::string id)
    {
        ValType t = symbol_table.get_value_type(id);

        if(t == ValType::NONE)
        {
            error_man.report_unknown_id(id, true);
        }

        return t;
    }

    // ----------------------------------------------------------
    //
    // ----------------------------------------------------------

    void Analyzer::build_function(Function *function)
    {
        std::cout << "Analyzer >> Building function : " << function->name << std::endl;

        // Ensure function is unique
        if(symbol_table.does_context_exist(function->name))
        {
            // Dies if not unique
            error_man.report_previously_declared(function->name);
        }

        // Create function context
        // Remove the last function's context
        symbol_table.new_context(function->name, true);

        // Place function parameters into context
        for(auto & p : function->params)
        {
            symbol_table.add_symbol(p.id, p.type);
        }

        // Tell code generator to start function with given parametrs
        code_gen.begin_function(function->name, function->params);

        // Iterate over function elements and visit them with *this
        for(auto & el : function->elements)
        {
            // Visiting elements will trigger analyzer to check particular element
            // for any errors that may be present
            el->visit(*this);

            // If no errors are present in the current statement, then we trigger code generation
            el->visit(code_gen);

            // Now that the item is constructed, we free the memory
            delete el;
        }

        // Tell code generator that we are done constructin the current function
        code_gen.end_function();

        // Function is constructed - and elements have been freed
        delete function;
    }

    // ----------------------------------------------------------
    //
    // ----------------------------------------------------------

    void Analyzer::accept(Assignment &stmt)
    {
        // Check assignment to see if data_type requires check
            // A check represents reassignment, so need to ensure that lhs
            // is reachable within the current context
            // If it requires check, we need to ensure that the ast represents
            // data that the type can handle, if it needs promotion

        // GO over ast and ensure that all data is the same type as we expect. If a string is found
        // for int/real item it better be a symbol in the variable map or we need to die
    }
}










/*


   void DEL_Driver::handle_assignment(Assignment * assignment)
   {
      std::cout << "Handle assignment for : " << assignment->lhs << std::endl;

      std::string r = evaluate(assignment->rhs);

      std::cout << "RES : " << r << std::endl;

      symbol_table.add_symbol(assignment->lhs, assignment->data_type);

   }

   std::string DEL_Driver::evaluate(AST * ast)
   {
      switch(ast->node_type)
      {
         case NodeType::ROOT: 
         { 
            return "root";
            break;
         }
         case NodeType::ADD : 
         { 
            std::cout << "Add : " << evaluate(ast->l) << " + " << evaluate(ast->r) << std::endl;
            break;
         }  
         case NodeType::SUB : 
         { 
            std::cout << "Sub : " << evaluate(ast->l) << " - " << evaluate(ast->r) << std::endl;
            break;
         }
         case NodeType::DIV : 
         { 
            std::cout << "Div : " << evaluate(ast->l) << " / " << evaluate(ast->r) << std::endl;
            break;
         }
         case NodeType::MUL : 
         { 
            std::cout << "Mul : " << evaluate(ast->l) << " * " << evaluate(ast->r) << std::endl;
            break;
         }
         case NodeType::MOD : 
         { 
            std::cout << "Mod : " << evaluate(ast->l) << " % " << evaluate(ast->r) << std::endl;
            break;
         }
         case NodeType::POW : 
         { 
            std::cout << "Pow : " << evaluate(ast->l) << " ^ " << evaluate(ast->r) << std::endl;
            break;
         }
         case NodeType::VAL : 
         { 
            return ast->value;
         }
         case NodeType::ID  : 
         { 
            return ast->value;
         }
         default:
            return "Its dead, jim";
      }
      return "Complete";
   }


*/