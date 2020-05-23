#ifndef NABLA_NHLL_PARSER_HPP
#define NABLA_NHLL_PARSER_HPP

#include "NhllLanguageIf.hpp"
#include "CompilerError.hpp"
#include "NhllVisitor.hpp"

#include <string>
#include <vector>

namespace NABLA
{
namespace COMPILERS
{
    class NhllParser
    {
    public:
        NhllParser(NhllLanguageIf & iface);
        ~NhllParser();

        void parseLine(std::string line);

    private:
    
        uint64_t line_number;
        uint16_t line_index;

        NhllLanguageIf * language_iface;
        

        void throwCompilerError(std::string error, uint16_t line_idx, std::vector<std::string> suggestions = {});

    };
}
}

#endif