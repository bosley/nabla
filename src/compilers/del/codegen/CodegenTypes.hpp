#ifndef DEL_CODEGEN_TYPES_HPP
#define DEL_CODEGEN_TYPES_HPP

#include <string>
#include <vector>
#include <stdint.h>

#include "Memory.hpp"

namespace DEL
{
namespace CODEGEN
{
namespace TYPES
{
    enum class DataClassification
    {
        INTEGER,
        DOUBLE,
        CHAR
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
        MOVE_BYTE,
        MOVE_WORD,

        RETURN,

        USE_RAW        // Use the given value (int or str val)
    };

    struct ParamInfo
    {
        uint64_t start_pos;
        uint64_t end_pos;
        uint16_t param_number;
    };

    class BaseInstruction
    {
    public:
        BaseInstruction(InstructionSet instruction) : instruction(instruction) {}
        InstructionSet instruction;
    };

    class StandardInstruction : public BaseInstruction
    {
    public:
        StandardInstruction(InstructionSet instruction, std::string value) : 
            BaseInstruction(instruction), value(value){}

        std::string value;
    };

    class CallInstruction : public BaseInstruction
    {
    public:
        CallInstruction(InstructionSet instruction, std::string destination, std::vector<ParamInfo> params) : 
                BaseInstruction(instruction), destination(destination), params(params) {}

        std::string destination;
        std::vector<ParamInfo> params;
    };


    class Assignment
    {
    public:
        std::string id;
        Memory::MemAlloc memory_info;
        DataClassification classification;
        std::vector<BaseInstruction*> instructions;

    };
}
}
}


#endif 