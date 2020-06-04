#ifndef DEL_CODE_GEN_HPP
#define DEL_CODE_GEN_HPP

#include "ast.hpp"
#include "Errors.hpp"
#include "SymbolTable.hpp"

namespace DEL
{
    //! \brief Code generator
    class Codegen : public Visitor
    {
    public:
        //! \brief Construct a code generator
        //! \param err The error manager
        //! \param symbolTable The program symbol table
        Codegen(Errors & err, SymbolTable & symbolTable);

        //! \brief Deconstrut the code generator
        ~Codegen();

        void begin_function(std::string name, std::vector<FunctionParam> params);

        void end_function();

        // From visitor
        void accept(Assignment &stmt) override;

    private:
        Errors & error_man;         // Error manager
        SymbolTable & symbol_table; // Symbol table
        bool building_function;
    };
}

#endif 