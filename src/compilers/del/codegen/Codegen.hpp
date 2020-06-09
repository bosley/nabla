#ifndef DEL_CODE_GEN_HPP
#define DEL_CODE_GEN_HPP

#include "Ast.hpp"
#include "Errors.hpp"
#include "SymbolTable.hpp"
#include "Memory.hpp"
#include "Intermediate.hpp"
#include "AsmSupport.hpp"

#include "FunctionRepresentation.hpp"

namespace DEL
{
    //! \brief Code generator
    class Codegen
    {
    public:
    
        //! \brief Construct a code generator
        //! \param err The error manager
        //! \param symbolTable The program symbol table
        Codegen(Errors & err, SymbolTable & symbolTable, Memory & memory);

        //! \brief Deconstrut the code generator
        ~Codegen();

        //! \brief Complete the generation of code
        //! \retval ASM generated by codegen
        std::vector<std::string> indicate_complete();

        //! \brief Begin function
        //! \param name The function name
        //! \param params The function's parameters
        //! \post The code generator will be set to start building a function
        void begin_function(std::string name, std::vector<FunctionParam> params);

        //! \brief End function
        //! \post The code generator will be ready to begin another function
        void end_function();

        //! \brief Generate an assignment
        //! \param assignment The instructions used to build an assignment
        void assignment(Intermediate::Assignment assignment);

        //! \brief Create a special return that doesn't return a value
        void null_return();

    private:
        Errors & error_man;         // Error manager
        SymbolTable & symbol_table; // Symbol table
        Memory & memory_man;        // Memory manager
        bool building_function;
        AsmSupport asm_support;

        uint64_t label_id;

        std::vector<std::string> program_init;
        std::vector<std::string> program_instructions;

        PARTS::FunctionRepresentation * current_function;

        void setup_integer(std::string id, std::string value);
        void setup_double(std::string id, std::string value);
        void setup_char(std::string id, std::string value);
    };
}

#endif 