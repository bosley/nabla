#ifndef NABLA_NHLL_COMPILER_IF_HPP
#define NABLA_NHLL_COMPILER_IF_HPP

#include <any>
#include <string>
#include "CompilerError.hpp"

namespace NABLA
{
namespace COMPILERS
{
    enum class VariableType
    {
        INTEGER,
        REAL,
        STRING,
        USER_DEFINED
    };

    class NhllLanguageIf
    {
    public:

        virtual void handle_error(NABLA::CompilerError compiler_error) = 0;

        virtual void process_let_stmt(std::string var, std::any value, VariableType type) = 0;

        virtual void process_use_stmt(std::string module_path, std::string as) = 0;
    };
}
}

#endif 