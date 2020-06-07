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

        //! \brief Complete the generation of code, and return the raw ASM
        std::vector<std::string> indicate_complete();

        void begin_function(std::string name, std::vector<FunctionParam> params);

        void end_function();

        void assignment(Intermediate::Assignment assignment);

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