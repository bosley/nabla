#ifndef NABLA_COMPILERS_LANGUAGE_NHLL_HPP
#define NABLA_COMPILERS_LANGUAGE_NHLL_HPP

#include <any>
#include "CompilerIf.hpp"
#include "CompilerError.hpp"
#include "NhllLanguageIf.hpp"
#include "NhllParser.hpp"

namespace NABLA
{
namespace COMPILERS
{
    class Nhll :  public NhllLanguageIf, public CompilerIf
    {
    public:
        Nhll();


        virtual CompiledPackage loadSource(std::string source_file) override;

    private:

        friend NhllParser;

        virtual void handle_error(NABLA::CompilerError compiler_error) override;

        virtual void process_let_stmt(std::string var, std::any value, VariableType type) override;
        
        virtual void process_use_stmt(std::string module_path, std::string as) override;

        NhllParser parser;
    };
}
}


#endif

