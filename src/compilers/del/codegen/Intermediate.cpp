#include "Intermediate.hpp"

#include <sstream>
#include <iterator>
#include <vector>
#include <iostream>

#define N_UNUSED(x) (void)x;

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

        bool is_only_number(std::string v)
        {
            try
            {
                double value = std::stod(v);
                N_UNUSED(value)
                return true;
            }
            catch(std::exception& e)
            {
                // Its not a number
            }
            return false;
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
        Assignment assignment;

        // We want to be particular about chars
        if(classification == AssignmentClassifier::CHAR)
        {
            assignment = build_assignment(tokens, 1);
        }
        else
        {
            assignment = build_assignment(tokens, memory_info.bytes_alloced);
        }


        // Indicate how raw values should be interpd
        assignment.assignment_classifier = classification;

        // Information regarding where to store result
        assignment.memory_info = memory_info;

        return assignment;
    }

    // ----------------------------------------------------------
    //
    // ----------------------------------------------------------

    void Intermediate::build_assignment_directive(Intermediate::Assignment & assignment, std::string directive_token, uint64_t byte_len)
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
            uint64_t start_pos = std::stoull(d_list[1]);
            uint64_t end_pos   = start_pos + std::stoull(d_list[2]);

            if(byte_len == 1)
            {
                assignment.instructions.push_back(
                    {
                        InstructionSet::LOAD_BYTE,
                        std::to_string(start_pos)
                    }
                );
            }
            else
            {
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
                    InstructionSet::STORE_WORD, // Get the result of the function call for assignment 
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

    Intermediate::Assignment Intermediate::build_assignment(std::vector<std::string> & tokens, uint64_t byte_len)
    {
        Intermediate::Assignment assignment;

        // Check all tokens for what they represent
        for(auto & token : tokens)
        {
            // Check for a directive
            if(token[0] == '#')
            {
                build_assignment_directive(assignment, token, byte_len);
            }
            // Check for char || int || double (raw values)
            else if(token[0] == '"' || is_only_number(token) )
            {
                assignment.instructions.push_back(
                    {
                        InstructionSet::USE_RAW,
                        token
                    }
                );
            }
            else
            {
                // Get the operation token
                assignment.instructions.push_back(
                    {
                        get_operation(token),
                        ""
                    }
                );
            }
        }

        // If its a return statement, we don't want to add a store command
        if(assignment.instructions.back().instruction == Intermediate::InstructionSet::RETURN)
        {
            return assignment;
        }

        Intermediate::InstructionSet final = (byte_len < 8) ? Intermediate::InstructionSet::STORE_BYTE : Intermediate::InstructionSet::STORE_WORD;

        // End of assignment trigger storage of result
        assignment.instructions.push_back(
            {
                final,
                ""
            }
        );
        return assignment;
    }

    // ----------------------------------------------------------
    // There HAS TO BE A BETTER WAY TO DO THIS
    // ----------------------------------------------------------

    Intermediate::InstructionSet Intermediate::get_operation(std::string token)
    {
        if(token == "ADD"   ) { return Intermediate::InstructionSet::ADD;    }
        if(token == "SUB"   ) { return Intermediate::InstructionSet::SUB;    }
        if(token == "MUL"   ) { return Intermediate::InstructionSet::MUL;    }
        if(token == "DIV"   ) { return Intermediate::InstructionSet::DIV;    }
        if(token == "MOD"   ) { return Intermediate::InstructionSet::MOD;    }
        if(token == "POW"   ) { return Intermediate::InstructionSet::POW;    }
        if(token == "LTE"   ) { return Intermediate::InstructionSet::LTE;    }
        if(token == "LT"    ) { return Intermediate::InstructionSet::LT;     }
        if(token == "GTE"   ) { return Intermediate::InstructionSet::GTE;    }
        if(token == "GT"    ) { return Intermediate::InstructionSet::GT;     }
        if(token == "EQ"    ) { return Intermediate::InstructionSet::EQ;     }
        if(token == "NE"    ) { return Intermediate::InstructionSet::NE;     }
        if(token == "LSH"   ) { return Intermediate::InstructionSet::LSH;    }
        if(token == "RSH"   ) { return Intermediate::InstructionSet::RSH;    }
        if(token == "NEGATE") { return Intermediate::InstructionSet::NEGATE; }
        if(token == "OR"    ) { return Intermediate::InstructionSet::OR;     }
        if(token == "AND"   ) { return Intermediate::InstructionSet::AND;    }
        if(token == "BW_OR" ) { return Intermediate::InstructionSet::BW_OR;  }
        if(token == "BW_XOR") { return Intermediate::InstructionSet::BW_XOR; }
        if(token == "BW_AND") { return Intermediate::InstructionSet::BW_AND; }
        if(token == "BW_NOT") { return Intermediate::InstructionSet::BW_NOT; }
        if(token == "RETURN") { return Intermediate::InstructionSet::RETURN; }
        std::cerr << "Developer error : Intermediate::InstructionSet Intermediate::get_integer_operation(std::string token)" << std::endl;
        exit(EXIT_FAILURE);
    }

}