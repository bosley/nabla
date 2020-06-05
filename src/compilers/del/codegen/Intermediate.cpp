#include "Intermediate.hpp"

#include <sstream>
#include <iterator>
#include <vector>
#include <iostream>

namespace DEL
{
    namespace
    {
        template<typename Numeric>
        inline static bool is_number(const std::string& s)
        {
            Numeric n;
            return((std::istringstream(s) >> n >> std::ws).eof());
        }
    }

    // ----------------------------------------------------------
    //
    // ----------------------------------------------------------

    Intermediate::Intermediate()
    {

    }

    // ----------------------------------------------------------
    //
    // ----------------------------------------------------------

    Intermediate::~Intermediate()
    {

    }

    // ----------------------------------------------------------
    //
    // ----------------------------------------------------------

    Intermediate::Assignment Intermediate::encode_postfix_assignment_expression(Memory::MemAlloc memory_info,  AssignmentClassifier classification, std::string expression)
    {
        // Build the expression into a string vector
        std::istringstream buf(expression);
        std::istream_iterator<std::string> beg(buf), end;
        std::vector<std::string> tokens(beg, end);

        // Build instructions for assignment
        Assignment assignment = build_assignment(tokens);

        // Indicate how raw values should be interpd
        assignment.assignment_classifier = classification;

        // Information regarding where to store result
        assignment.memory_info = memory_info;

        return assignment;
    }

    // ----------------------------------------------------------
    //
    // ----------------------------------------------------------

    void Intermediate::build_assignment_directive(Intermediate::Assignment & assignment, std::string directive_token)
    {
        // Remove the directive indicator
        directive_token = directive_token.substr(1, directive_token.size());

        // Get the directive tokens
        std::string segment;
        std::vector<std::string> d_list;
        std::stringstream ss(directive_token);
        while(std::getline(ss, segment, ':'))
        {
            d_list.push_back(segment);
        }

        // These are generated by us, so if they don't match then we messed up, not the user
        if(d_list[0] == "ID")
        {
            uint64_t start_pos = std::stoll(d_list[1]);
            uint64_t end_pos   = std::stoll(d_list[2]);

            while(start_pos < end_pos)
            {
                assignment.instructions.push_back(
                    {
                        InstructionSet::LOAD_WORD,
                        std::to_string(start_pos)
                    }
                );
                start_pos += 8; // Inc by word
            }
        }
        else if(d_list[0] == "FCALL")
        {
            assignment.instructions.push_back(
                {
                    InstructionSet::CALL,
                    d_list[1]               // Should contain function name to call
                }
            );
            assignment.instructions.push_back(
                {
                    InstructionSet::GET_RESULT, // Get the result of the function call for assignment 
                    "" // No info for this
                }
            );
        }
        else
        {
            std::cerr << "Internal developer error in Intermediate::Assignment::build_directive" << std::endl;
            exit(EXIT_FAILURE);
        }
    }
    
    // ----------------------------------------------------------
    //
    // ----------------------------------------------------------

    Intermediate::Assignment Intermediate::build_assignment(std::vector<std::string> & tokens)
    {
        Intermediate::Assignment assignment;

        // Check all tokens for what they represent
        for(auto & token : tokens)
        {
            // Check for a directive
            if(token[0] == '#')
            {
                build_assignment_directive(assignment, token);
                continue;
            }

            // Check for char || int || double (raw values)
            if(token[0] == '"' || is_number<int>(token) || is_number<double>(token))
            {
                assignment.instructions.push_back(
                    {
                        InstructionSet::USE_RAW,
                        token
                    }
                );
                continue;
            }

            // Get the operation token
            assignment.instructions.push_back(
                {
                    get_operation(token),
                    ""
                }
            );
        }
        std::cout << std::endl;

        return assignment;
    }

    // ----------------------------------------------------------
    // There HAS TO BE A BETTER WAY TO DO THIS
    // ----------------------------------------------------------

    Intermediate::InstructionSet Intermediate::get_operation(std::string token)
    {
        // Defined by use in del_parser.yy
        if(token == "+") { return Intermediate::InstructionSet::ADD; }
        if(token == "-") { return Intermediate::InstructionSet::SUB; }
        if(token == "*") { return Intermediate::InstructionSet::MUL; }
        if(token == "/") { return Intermediate::InstructionSet::DIV; }
        if(token == "%") { return Intermediate::InstructionSet::MOD; }
        if(token == "^") { return Intermediate::InstructionSet::POW; }

        // Defined in del_lexer.l
        if(token == "<=" ) { return Intermediate::InstructionSet::LTE;    }
        if(token == "<"  ) { return Intermediate::InstructionSet::LT;     }
        if(token == ">=" ) { return Intermediate::InstructionSet::GTE;    }
        if(token == ">"  ) { return Intermediate::InstructionSet::GT;     }
        if(token == "==" ) { return Intermediate::InstructionSet::EQ;     }
        if(token == "!=" ) { return Intermediate::InstructionSet::NE;     }
        if(token == "<<" ) { return Intermediate::InstructionSet::LSH;    }
        if(token == ">>" ) { return Intermediate::InstructionSet::RSH;    }
        if(token == "!"  ) { return Intermediate::InstructionSet::NEGATE; }
        if(token == "||" ) { return Intermediate::InstructionSet::OR;     }
        if(token == "&&" ) { return Intermediate::InstructionSet::AND;    }
        if(token == "or" ) { return Intermediate::InstructionSet::BW_OR;  }
        if(token == "xor") { return Intermediate::InstructionSet::XOR;    }
        if(token == "and") { return Intermediate::InstructionSet::BW_AND; }
        if(token == "not") { return Intermediate::InstructionSet::NOT;    }

        std::cerr << "Developer error : Intermediate::InstructionSet Intermediate::get_integer_operation(std::string token)" << std::endl;
        exit(EXIT_FAILURE);
    }

}