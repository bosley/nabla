#ifndef NABLA_NHLL_PARSER_HPP
#define NABLA_NHLL_PARSER_HPP

#include "NhllLanguageIf.hpp"
#include "CompilerError.hpp"

#include <string>
#include <vector>
#include <regex>

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
        NhllLanguageIf * language_iface;

        void process_line();

        std::vector<std::string> current_line;

        struct MatchCall
        {
            std::regex reg;
            std::function<void()> call;
        };

        std::vector<MatchCall> parserMethods;
        std::vector<std::string> keywords;



        void parse_use();
        void parse_let();
    };
}
}

#endif