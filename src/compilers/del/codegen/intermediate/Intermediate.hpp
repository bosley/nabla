#ifndef DEL_INTERMEDIATE_HPP
#define DEL_INTERMEDIATE_HPP

#include <stdint.h>
#include <string>
#include <vector>
#include "Memory.hpp"
#include "Codegen.hpp"
#include "SymbolTable.hpp"
#include "Types.hpp"
#include "IntermediateTypes.hpp"

namespace DEL
{
    //! \class Intermediate
    //! \brief The intermediate step generator that builds instructions for code generator to
    //!        ... generate code...
    class Intermediate
    {
    public:
        //! \brief Create the Intermediate 
        Intermediate(SymbolTable & symbol_table, Memory & memory_man, Codegen & code_gen);

        //! \brief Destruct the Intermediate
        ~Intermediate();


        void issue_start_function(std::string name, std::vector<FunctionParam> params);

        void issue_end_function();

        void issue_null_return();

        //! \brief Issue an assignment command to the code generator
        //! \param id The id being assigned
        //! \param memory_info The memory information for the resulting assignment
        //! \param classification The classification of the assignment
        //! \param expression The expression to be computed
        void issue_assignment(std::string id, Memory::MemAlloc memory_info, INTERMEDIATE::TYPES::AssignmentClassifier classification, std::string postfix_expression);

    private:

        INTERMEDIATE::TYPES::Assignment encode_postfix_assignment_expression(Memory::MemAlloc memory_info, INTERMEDIATE::TYPES::AssignmentClassifier classification, std::string expression);
    
        SymbolTable & symbol_table;
        Memory & memory_man;
        Codegen & code_gen;

        void build_assignment_directive(INTERMEDIATE::TYPES::Assignment & assignment, std::string directive_token, uint64_t byte_len);

        INTERMEDIATE::TYPES::Assignment build_assignment(std::vector<std::string> & tokens, uint64_t byte_len);

        INTERMEDIATE::TYPES::InstructionSet get_operation(std::string token);
    };
}

#endif