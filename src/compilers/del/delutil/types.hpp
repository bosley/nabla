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
        ID
    };

    enum class ValType
    {
        INTEGER,
        REAL,
        CHAR,
        STRING,
        FUNCTION,
        REQ_CHECK,
        NONE
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
        case ValType::INTEGER  : return "Integer";
        case ValType::REAL     : return "Real";
        case ValType::CHAR     : return "Char";
        case ValType::STRING   : return "String";
        case ValType::FUNCTION : return "Function";
        case ValType::NONE     : return "None";
        case ValType::REQ_CHECK: return "RequiresCheck";
        default                : return "Unknown";
        }
        return "Unknown";
    }
}

#endif