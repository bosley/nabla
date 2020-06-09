#include "Intermediate.hpp"

#include <sstream>
#include <iterator>
#include <vector>
#include <iostream>
#include <libnabla/endian.hpp>
#include <libnabla/util.hpp>

#include "EnDecode.hpp"

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

        std::vector<std::string> split(std::string s, char c)
        {
            std::string segment;
            std::vector<std::string> list;
            std::stringstream ss(s);
            while(std::getline(ss, segment, c))
            {
                list.push_back(segment);
            }
            return list;
        }
    }

    // ----------------------------------------------------------
    //
    // ----------------------------------------------------------

    Intermediate::Intermediate(SymbolTable & symbol_table, Memory & memory_man, Codegen & code_gen) : 
            symbol_table(symbol_table), memory_man(memory_man), code_gen(code_gen)
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

    void Intermediate::issue_start_function(std::string name, std::vector<FunctionParam> params)
    {
        code_gen.begin_function(name, params);
    }

    // ----------------------------------------------------------
    //
    // ----------------------------------------------------------

    void Intermediate::issue_end_function()
    {
        code_gen.end_function();
    }

    // ----------------------------------------------------------
    //
    // ----------------------------------------------------------

    void Intermediate::issue_null_return()
    {
        code_gen.null_return();
    }

    // ----------------------------------------------------------
    //
    // ----------------------------------------------------------

    void Intermediate::issue_assignment(std::string id, Memory::MemAlloc memory_info, INTERMEDIATE::TYPES::AssignmentClassifier classification, std::string postfix_expression)
    {
        // Build the instruction set
        INTERMEDIATE::TYPES::Assignment assignment = encode_postfix_assignment_expression(memory_info, classification, postfix_expression);
        assignment.id = id;

        // Issue the command
        code_gen.assignment(assignment);
    }

    // ----------------------------------------------------------
    //
    // ----------------------------------------------------------

    INTERMEDIATE::TYPES::Assignment Intermediate::encode_postfix_assignment_expression(Memory::MemAlloc memory_info,  INTERMEDIATE::TYPES::AssignmentClassifier classification, std::string expression)
    {
        // Build the expression into a string vector
        std::istringstream buf(expression);
        std::istream_iterator<std::string> beg(buf), end;
        std::vector<std::string> tokens(beg, end);

        // Build instructions for assignment
        INTERMEDIATE::TYPES::Assignment assignment;

        // We want to be particular about chars
        if(classification == INTERMEDIATE::TYPES::AssignmentClassifier::CHAR)
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

    void Intermediate::build_assignment_directive(INTERMEDIATE::TYPES::Assignment & assignment, std::string directive_token, uint64_t byte_len)
    {
        EnDecode endecode(memory_man);

        INTERMEDIATE::TYPES::Directive directive = endecode.decode_directive(directive_token);

        // Figure out what type the directive is directing us to do
        switch(directive.type)
        {
            // Handle an ID
            case INTERMEDIATE::TYPES::DirectiveType::ID:
            {
                // If the item is only 1 byte, call load byte on the start position
                if(byte_len == 1){  assignment.instructions.push_back( { INTERMEDIATE::TYPES::InstructionSet::LOAD_BYTE, std::to_string(directive.allocation[0].start_pos) }); }
                else
                {
                    // If the item is multiple bytes, then we load words until we've loaded everything
                    while(directive.allocation[0].start_pos < directive.allocation[0].end_pos)
                    {
                        assignment.instructions.push_back(
                            {
                                INTERMEDIATE::TYPES::InstructionSet::LOAD_WORD,
                                std::to_string(directive.allocation[0].start_pos)
                            }
                        );
                        directive.allocation[0].start_pos += 8; // Inc by word
                    }
                }
                break;
            }

            // Handle a call
            case INTERMEDIATE::TYPES::DirectiveType::CALL:
            {
                std::cerr << "GOT THE CALL - NOT DONE YET";
                exit(EXIT_FAILURE);
                break;
            }

            default:
            {
                // Shouldn't ever happen 
                std::cerr << "Developer Error >>> Intermediate was handed a wonky token from the EnDecoder" << std::endl;
                exit(EXIT_FAILURE);
                break; 
            }
        }

        /*
        // Remove the directive indicator
        directive_token = directive_token.substr(1, directive_token.size());

        // Get the directive tokens
        std::vector<std::string> d_list = split(directive_token, ':');

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
        else if(d_list[0] == "CALL")
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
            std::cerr << "Internal developer error in INTERMEDIATE::TYPES::Assignment::build_directive" << std::endl;
            exit(EXIT_FAILURE);
        }
        */
    }
    
    // ----------------------------------------------------------
    //
    // ----------------------------------------------------------

    INTERMEDIATE::TYPES::Assignment Intermediate::build_assignment(std::vector<std::string> & tokens, uint64_t byte_len)
    {
        INTERMEDIATE::TYPES::Assignment assignment;

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
                        INTERMEDIATE::TYPES::InstructionSet::USE_RAW,
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
        if(assignment.instructions.back().instruction == INTERMEDIATE::TYPES::InstructionSet::RETURN)
        {
            return assignment;
        }

        INTERMEDIATE::TYPES::InstructionSet final = (byte_len < 8) ? 
            INTERMEDIATE::TYPES::InstructionSet::STORE_BYTE : 
            INTERMEDIATE::TYPES::InstructionSet::STORE_WORD;

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

    INTERMEDIATE::TYPES::InstructionSet Intermediate::get_operation(std::string token)
    {
        if(token == "ADD"   ) { return INTERMEDIATE::TYPES::InstructionSet::ADD;    }
        if(token == "SUB"   ) { return INTERMEDIATE::TYPES::InstructionSet::SUB;    }
        if(token == "MUL"   ) { return INTERMEDIATE::TYPES::InstructionSet::MUL;    }
        if(token == "DIV"   ) { return INTERMEDIATE::TYPES::InstructionSet::DIV;    }
        if(token == "MOD"   ) { return INTERMEDIATE::TYPES::InstructionSet::MOD;    }
        if(token == "POW"   ) { return INTERMEDIATE::TYPES::InstructionSet::POW;    }
        if(token == "LTE"   ) { return INTERMEDIATE::TYPES::InstructionSet::LTE;    }
        if(token == "LT"    ) { return INTERMEDIATE::TYPES::InstructionSet::LT;     }
        if(token == "GTE"   ) { return INTERMEDIATE::TYPES::InstructionSet::GTE;    }
        if(token == "GT"    ) { return INTERMEDIATE::TYPES::InstructionSet::GT;     }
        if(token == "EQ"    ) { return INTERMEDIATE::TYPES::InstructionSet::EQ;     }
        if(token == "NE"    ) { return INTERMEDIATE::TYPES::InstructionSet::NE;     }
        if(token == "LSH"   ) { return INTERMEDIATE::TYPES::InstructionSet::LSH;    }
        if(token == "RSH"   ) { return INTERMEDIATE::TYPES::InstructionSet::RSH;    }
        if(token == "NEGATE") { return INTERMEDIATE::TYPES::InstructionSet::NEGATE; }
        if(token == "OR"    ) { return INTERMEDIATE::TYPES::InstructionSet::OR;     }
        if(token == "AND"   ) { return INTERMEDIATE::TYPES::InstructionSet::AND;    }
        if(token == "BW_OR" ) { return INTERMEDIATE::TYPES::InstructionSet::BW_OR;  }
        if(token == "BW_XOR") { return INTERMEDIATE::TYPES::InstructionSet::BW_XOR; }
        if(token == "BW_AND") { return INTERMEDIATE::TYPES::InstructionSet::BW_AND; }
        if(token == "BW_NOT") { return INTERMEDIATE::TYPES::InstructionSet::BW_NOT; }
        if(token == "RETURN") { return INTERMEDIATE::TYPES::InstructionSet::RETURN; }
        std::cerr << "Developer error : Intermediate::InstructionSet Intermediate::get_integer_operation(std::string token)" << std::endl;
        exit(EXIT_FAILURE);
    }




}