#ifndef DEL_TYPES_HPP
#define DEL_TYPES_HPP

#include <string>

namespace DEL
{
    enum class NodeType
    {
        ROOT,
        ADD,
        SUB,
        DIV,
        MUL,
        MOD,
        POW,
        VAL,
        ID,

        LTE,
        GTE,
        GT ,
        LT ,
        EQ ,
        NE ,
        LSH,
        RSH,
        BW_OR,
        BW_XOR,
        BW_AND,
        BW_NOT,
        OR,
        AND,
        NEGATE

    };

    enum class ValType
    {
        INTEGER,    // An actual integer
        REAL,       // An actual real
        CHAR,       // An actual char

        STRUCT,
        FUNCTION_CALL,

        PTR,
        REF,

        STRING,     // Identifiers
        FUNCTION,   // A function declaration
        REQ_CHECK,  // Requires check by analyzer for value type (reassignment)
        NONE        // Nothing
    };

    struct FunctionParam
    {
        ValType type;
        std::string id;
    };

    static inline std::string ValType_to_string(ValType v)
    {
        switch(v)
        {
        case ValType::INTEGER       : return "Integer";
        case ValType::REAL          : return "Real";
        case ValType::CHAR          : return "Char";
        case ValType::STRING        : return "String";
        case ValType::FUNCTION      : return "Function";
        case ValType::NONE          : return "None";
        case ValType::REQ_CHECK     : return "RequiresCheck";
        case ValType::STRUCT        : return "Struct";
        case ValType::FUNCTION_CALL : return "FunctionCall";
        case ValType::PTR           : return "Pointer";
        case ValType::REF           : return "Reference";
        default                     : return "Unknown";
        }
        return "Unknown";
    }
}

#endif