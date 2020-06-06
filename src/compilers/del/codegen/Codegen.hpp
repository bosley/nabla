#ifndef DEL_CODE_GEN_HPP
#define DEL_CODE_GEN_HPP

#include "Ast.hpp"
#include "Errors.hpp"
#include "SymbolTable.hpp"
#include "Memory.hpp"
#include "Intermediate.hpp"

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

        void begin_function(std::string name, std::vector<FunctionParam> params);

        void end_function();

        void add_build_in_math_functions();

        void assignment(Intermediate::Assignment assignment);

    private:
        Errors & error_man;         // Error manager
        SymbolTable & symbol_table; // Symbol table
        Memory & memory_man;        // Memory manager
        bool building_function;

        bool has_included_math_functions;

        uint64_t label_id;

        std::vector<std::string> program_init;
        std::vector<std::string> program_instructions;

        void setup_integer(std::string id, std::string value);
        void setup_double(std::string id, std::string value);
        void setup_string(std::string id, std::string value);
    };
}

#endif 