#ifndef DEL_INTERMEDIATE_TYPES_HPP
#define DEL_INTERMEDIATE_TYPES_HPP

namespace DEL
{
namespace INTERMEDIATE
{
namespace TYPES
{

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
        LOAD_PARAM,

        RETURN,

        USE_RAW        // Use the given value (int or str val)
    };

    enum class DirectiveType
    {
        ID, CALL
    };

    struct DirectiveAllocation
    {
        uint64_t start_pos;
        uint64_t end_pos;
    };

    struct Directive
    {
        DirectiveType type;
        std::vector<DirectiveAllocation> allocation;
    };

    struct ParamInfo
    {
        uint64_t start_pos;
        uint64_t end_pos;
        uint16_t param_number;
    };

    //! \brief An instruction / value pair
    struct AssignemntInstruction
    {
        InstructionSet instruction;
        std::string value;

        // If the instruction is a parameter, we need to encode some extra information
        //ParamInfo p_info;
    };
    
    //! \brief An assignment representation for the codegen to create an assignment
    struct Assignment
    {
        std::string id;
        Memory::MemAlloc memory_info;
        AssignmentClassifier assignment_classifier;
        std::vector<AssignemntInstruction> instructions;
    };

}
}
}


#endif 