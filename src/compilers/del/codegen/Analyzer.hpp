#ifndef DEL_ANALYZER_HPP
#define DEL_ANALYZER_HPP

#include "ast.hpp"
#include "Errors.hpp"
#include "SymbolTable.hpp"
#include "Codegen.hpp"

namespace DEL
{
    //! \brief The code analyzer
    class Analyzer : public Visitor
    {
    public:

        //! \brief Construct an analyzer
        //! \param err The error manager
        //! \param The symbol table
        Analyzer(Errors & err, SymbolTable & symbolTable, Codegen & code_gen);

        //! \brief Deconstruct tha analyzer
        ~Analyzer();

        //! \brief Build a function
        //! \param function The function to build
        void build_function(Function *function);

        // From visitor
        void accept(Assignment &stmt) override;

    private:

        void ensure_unique_symbol(std::string id);

        void ensure_id_in_current_context(std::string id, std::vector<ValType> allowed);

        ValType get_id_type(std::string id);

        Errors & error_man;             // Error manager
        SymbolTable & symbol_table;     // Symbol table
        Codegen & code_gen;             // Code generator

    };
}

#endif 