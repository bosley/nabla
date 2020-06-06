#ifndef DEL_INTERMEDIATE_HPP
#define DEL_INTERMEDIATE_HPP

#include <stdint.h>
#include <string>
#include <vector>
#include "Memory.hpp"

namespace DEL
{
    class Intermediate
    {
    public:
        Intermediate();
        ~Intermediate();


        enum class AssignmentClassifier
        {
            INTEGER, DOUBLE, CHAR //, STRUCT, STRING
        };

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

            USE_RAW        // Use the given value (int or str val)

        };


        struct AssignemntInstruction
        {
            Intermediate::InstructionSet instruction;
            std::string value;
        };
        
        struct Assignment
        {
            std::string id;
            Memory::MemAlloc memory_info;
            AssignmentClassifier assignment_classifier;
            std::vector<AssignemntInstruction> instructions;
        };

        Assignment encode_postfix_assignment_expression(Memory::MemAlloc memory_info, AssignmentClassifier classification, std::string expression);
    
    
    private:

        void build_assignment_directive(Intermediate::Assignment & assignment, std::string directive_token);

        Assignment build_assignment(std::vector<std::string> & tokens);

        InstructionSet get_operation(std::string token);
    };
}

#endif