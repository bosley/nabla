#ifndef DEL_ANALYZER_HPP
#define DEL_ANALYZER_HPP

#include "Ast.hpp"
#include "Errors.hpp"
#include "SymbolTable.hpp"
#include "Codegen.hpp"
#include "Intermediate.hpp"

namespace DEL
{
    //! \brief The code analyzer
    class Analyzer : public Visitor
    {
    public:

        //! \brief Construct an analyzer
        //! \param err The error manager
        //! \param The symbol table
        Analyzer(Errors & err, SymbolTable & symbolTable, Codegen & code_gen, Memory & memory);

        //! \brief Deconstruct tha analyzer
        ~Analyzer();

        //! \brief Build a function
        //! \param function The function to build
        void build_function(Function *function);

        // From visitor
        void accept(Assignment &stmt) override;
        void accept(ReturnStmt &stmt) override;
        void accept(Call       &stmt) override;

    private:

        void ensure_unique_symbol(std::string id);

        void ensure_id_in_current_context(std::string id, std::vector<ValType> allowed);

        ValType get_id_type(std::string id);

        void validate_call(Call & stmt);

        // Check that a given value is valid within the scope of an assignment 
        void check_value_is_valid_for_assignment(ValType type_to_check, Intermediate::AssignmentClassifier & classifier, ValType & assignee_type, std::string & id);

        // Validate an assignment ast
        std::string validate_assignment_ast(AST * ast, Intermediate::AssignmentClassifier & classifier, ValType & assignee_type, std::string & id);

        Errors & error_man;             // Error manager
        SymbolTable & symbol_table;     // Symbol table
        Codegen & code_gen;             // Code generator
        Memory & memory_man;            // Memory manager

        Intermediate intermediate_rep;

        Function * current_function;

        struct FunctionWatch
        {
            bool has_return;
        };

        FunctionWatch function_watcher;

    };
}

#endif 