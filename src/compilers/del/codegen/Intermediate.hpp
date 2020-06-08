#ifndef DEL_INTERMEDIATE_HPP
#define DEL_INTERMEDIATE_HPP

#include <stdint.h>
#include <string>
#include <vector>
#include "Memory.hpp"
#include "types.hpp"

namespace DEL
{
    //! \class Intermediate
    //! \brief The intermediate step generator that builds instructions for code generator to
    //!        ... generate code...
    class Intermediate
    {
    public:
        //! \brief Create the Intermediate 
        Intermediate();

        //! \brief Destruct the Intermediate
        ~Intermediate();

        //! \brief Classification of an assignment (what it should result in)
        enum class AssignmentClassifier
        {
            INTEGER, DOUBLE, CHAR //, STRUCT, STRING
        };

        //! \brief A set of instructions for the code generator to use in the processing of tokens
        enum class InstructionSet
        {
            // Arithmatic
            ADD, SUB, DIV, 
            MUL, RSH, LSH, 
            
            BW_OR, BW_NOT, BW_XOR, BW_AND,

            // Comparison
            LTE, LT, GTE, GT, EQ, NE, OR, AND, NEGATE, 

            // Built-in
            POW,

            MOD,

            // Load / Store
            LOAD_BYTE,
            STORE_BYTE,
            LOAD_WORD,
            STORE_WORD,

            CALL,

            RETURN,

            USE_RAW        // Use the given value (int or str val)

        };

        //! \brief An instruction / value pair
        struct AssignemntInstruction
        {
            Intermediate::InstructionSet instruction;
            std::string value;
        };
        
        //! \brief An assignment representation for the codegen to create an assignment
        struct Assignment
        {
            std::string id;
            Memory::MemAlloc memory_info;
            AssignmentClassifier assignment_classifier;
            std::vector<AssignemntInstruction> instructions;
        };

        //! \brief Function parameters converted and encoded for codegen
        struct AddressedFunctionParam
        {
            std::string id;
            uint64_t size;
            uint64_t address;
        };

        //! \brief Take a postfix (RPN) instruction and create an Intermediate::Assignment 
        //! \param memory_info The memory information for the resulting assignment
        //! \param classification The classification of the assignment
        //! \param expression The expression to be generated into an assignment
        Assignment encode_postfix_assignment_expression(Memory::MemAlloc memory_info, AssignmentClassifier classification, std::string expression);
    
        //! \brief Convert a list of function call parameters to an addressed parameter
        //! \param params The parameters to convert
        //! \returns Returns a list of addressed parameters
        std::vector<AddressedFunctionParam> convert_call_params(std::vector<FunctionParam> params);

    private:

        void build_assignment_directive(Intermediate::Assignment & assignment, std::string directive_token, uint64_t byte_len);

        Assignment build_assignment(std::vector<std::string> & tokens, uint64_t byte_len);

        InstructionSet get_operation(std::string token);
    };
}

#endif