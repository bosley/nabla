#include "NhllParser.hpp"

#include <iostream>

namespace
{
    // -----------------------------------------------
    //
    // -----------------------------------------------

    inline std::vector<std::string> chunkLine(std::string line) 
    {
        std::vector<std::string> chunks;

        bool inSubStr = false;
        std::string token = "";

        // Why do they even ever use this ? ugh
        if (!line.empty() && line[line.size() - 1] == '\r'){
            line.erase(line.size() - 1);
        }

        for(unsigned i = 0; i < line.length(); i++) {

            if (line[i] == '"') {
                inSubStr ? inSubStr = false : inSubStr = true;
            }

            if ((line[i] == ' ' || line[i] == '\t' || line[i] == '\n') && !inSubStr){
                if (token.length() > 0) {
                    chunks.push_back(token);
                    token.clear();
                } 
            } else {
                token = token + line[i];
            }
        }

        if (token.length() > 0) {
            chunks.push_back(token);
        }
        return chunks;
    }
    
    // -----------------------------------------------
    //
    // -----------------------------------------------

    inline std::string rtrim(std::string &line) 
    {
        line.erase(std::find_if(line.rbegin(), line.rend(), 
        [](int ch) 
        {
            return !std::isspace(ch);
        }).base(), line.end());
        return line;
    }

    // -----------------------------------------------
    //
    // -----------------------------------------------

    inline std::string ltrim (std::string &line)
    {
        line.erase(line.begin(),find_if_not(line.begin(),line.end(),
        [](int c)
        {
            return isspace(c);
        }));
        return line;
    }

    // -----------------------------------------------
    //
    // -----------------------------------------------

    inline std::vector<std::string> getStatements(std::string line)
    {
        std::string statement;
        std::stringstream liq(line);
        std::vector<std::string> statmentList;

        while(std::getline(liq, statement, ';'))
        {
            statmentList.push_back(ltrim(statement));

            std::cout << "STATEMENT : " << ltrim(statement) << std::endl;
        }
        return statmentList;
    }
}


namespace NABLA
{
namespace COMPILERS
{

    //
    //  ------------------------------------------- NHLL PARSER -------------------------------------------------
    //

    NhllParser::NhllParser(NhllLanguageIf & iface) : line_number(0), language_iface(&iface)
    {
        keywords = {
            "let", "use", "as"
        };

        parserMethods = {
            MatchCall{ std::regex("^let$"), std::bind(&NhllParser::parse_let, this) },
            MatchCall{ std::regex("^use$"), std::bind(&NhllParser::parse_use, this) }
        };
    }

    // -----------------------------------------------
    //
    // -----------------------------------------------

    NhllParser::~NhllParser()
    {

    }

    // -----------------------------------------------
    //
    // -----------------------------------------------

    void NhllParser::parseLine(std::string line)
    {
        //  Sep by ';' to get multiple statements per line
        //
        std::vector<std::string> statements = getStatements(line);

        //  Iterate all statements and parse
        //
        for(auto & statement : statements)
        {
            current_line = chunkLine(statement);

            process_line();
        }

    }

    // -----------------------------------------------
    //
    // -----------------------------------------------

    void NhllParser::process_line()
    {
        line_number++;

        // Ignore empty lines
        if(current_line.size() == 0)
        {
            return;
        }

        //  Figure out what the line is trying to do
        //
        bool found = false;
        for(uint64_t idx = 0; !found && idx < parserMethods.size(); idx++)
        {
            if(std::regex_match(current_line[0], parserMethods[idx].reg))
            {
                parserMethods[idx].call();
                found = true;
            }
        }

        //  If something wasn't matched, call it an error
        //
        if(!found)
        {
            CompilerError compiler_error;
            compiler_error.what = "Unhandled syntax";
            compiler_error.line_number = line_number;
            compiler_error.line_index  = 0;
            language_iface->handle_error(compiler_error);
        }
    }

    // -----------------------------------------------
    //
    // -----------------------------------------------

    void NhllParser::parse_let()
    {
        std::cout << "PARSE LET " << std::endl;

        // Remove let

        // find the '='

        // validate lhs - ensure is a variable 

        // validate rhs - [validate expression] ensure no keywords, ensure all sepd by ops or () 

        // Call into language_iface - It will check to ensure that all variables
        // currently exist, and whatnot

    }

    // -----------------------------------------------
    //
    // -----------------------------------------------

    void NhllParser::parse_use()
    {
        std::cout << "PARSE USE " << std::endl;
    }

}
}