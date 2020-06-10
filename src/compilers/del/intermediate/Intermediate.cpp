#include "Intermediate.hpp"

#include <sstream>
#include <iterator>
#include <vector>
#include <iostream>
#include <libnabla/endian.hpp>
#include <libnabla/util.hpp>
#include "CodegenTypes.hpp"
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

    Intermediate::Intermediate(Memory & memory_man, Codegen & code_gen) : 
            memory_man(memory_man), code_gen(code_gen)
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
        std::vector<CODEGEN::TYPES::ParamInfo> codegen_params;

        // Its important to start at 8. THE GS is byte-wise, we are operating word-wise
        uint16_t gs_param_start = 8;
        for(auto & p : params)
        {
            Memory::MemAlloc mem_info = memory_man.get_mem_info(p.id);

            codegen_params.push_back(CODEGEN::TYPES::ParamInfo{
                mem_info.start_pos,
                mem_info.start_pos + mem_info.bytes_alloced,
                gs_param_start
            });
            gs_param_start += 8;
        }

        code_gen.begin_function(name, codegen_params);
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

    void Intermediate::issue_direct_call(std::string encoded_call)
    {
        // Create the command
        CODEGEN::TYPES::Command command;

        // We know its a call directive, so we pass it to build directive
        build_assignment_directive(command, encoded_call, 1);

        // Because the standard case is to expect a return value, we need to edit the command before
        // We send it off
        CODEGEN::TYPES::CallInstruction * c = static_cast<CODEGEN::TYPES::CallInstruction *>(command.instructions.back());

        // We don't expect return values from direct calls
        c->expect_return_value = false;

        // Execute the call command
        code_gen.execute_command(command);
    }

    // ----------------------------------------------------------
    //
    // ----------------------------------------------------------

    void Intermediate::issue_assignment(std::string id, Memory::MemAlloc memory_info, INTERMEDIATE::TYPES::AssignmentClassifier classification, std::string postfix_expression)
    {
        // Build the instruction set
        CODEGEN::TYPES::Command command = encode_postfix_assignment_expression(memory_info, classification, postfix_expression);
        command.id = id;

        // Issue the command
        code_gen.execute_command(command);

        // Clean out the instructions
        for(auto & i : command.instructions)
        {
            delete i;
        }
    }

    // ----------------------------------------------------------
    //
    // ----------------------------------------------------------

    CODEGEN::TYPES::Command Intermediate::encode_postfix_assignment_expression(Memory::MemAlloc memory_info,  INTERMEDIATE::TYPES::AssignmentClassifier classification, std::string expression)
    {
        // Build the expression into a string vector
        std::istringstream buf(expression);
        std::istream_iterator<std::string> beg(buf), end;
        std::vector<std::string> tokens(beg, end);

        // Build instructions for command
        CODEGEN::TYPES::Command command;

        // We want to be particular about chars
        if(classification == INTERMEDIATE::TYPES::AssignmentClassifier::CHAR)
        {
            command = build_assignment(tokens, 1);
        }
        else
        {
            command = build_assignment(tokens, memory_info.bytes_alloced);
        }

        // Indicate how raw values should be interpd
        switch(classification)
        {
        case INTERMEDIATE::TYPES::AssignmentClassifier::CHAR:    command.classification = CODEGEN::TYPES::DataClassification::CHAR;    break;
        case INTERMEDIATE::TYPES::AssignmentClassifier::INTEGER: command.classification = CODEGEN::TYPES::DataClassification::INTEGER; break;
        case INTERMEDIATE::TYPES::AssignmentClassifier::DOUBLE:  command.classification = CODEGEN::TYPES::DataClassification::DOUBLE;  break;
        default: std::cerr << "Devloper Error >>> Intermediate::encode_postfix_assignment_expression() classification switch reached default : " << std::endl;
                 exit(EXIT_FAILURE);
                 break;
        }

        // Information regarding where to store result
        command.memory_info = memory_info;

        return command;
    }

    // ----------------------------------------------------------
    //
    // ----------------------------------------------------------

    void Intermediate::build_assignment_directive(CODEGEN::TYPES::Command & command, std::string directive_token, uint64_t byte_len)
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
                if(byte_len == 1)
                { 
                    command.instructions.push_back(
                        new CODEGEN::TYPES::AddressValueInstruction(CODEGEN::TYPES::InstructionSet::LOAD_BYTE,
                            directive.allocation[0].start_pos
                        )
                    );
                }
                else
                {
                    // If the item is multiple bytes, then we load words until we've loaded everything
                    while(directive.allocation[0].start_pos < directive.allocation[0].end_pos)
                    {
                        command.instructions.push_back(
                            new CODEGEN::TYPES::AddressValueInstruction(CODEGEN::TYPES::InstructionSet::LOAD_WORD,
                                directive.allocation[0].start_pos
                            )
                        );
                        directive.allocation[0].start_pos += 8; // Inc by word
                    }
                }
                break;
            }

            // Handle a call
            case INTERMEDIATE::TYPES::DirectiveType::CALL:
            {
                // Go through call and create CODEGEN::TYPES::MoveInstructions to move
                // local variables to the parameter passing zone
                uint64_t param_gs_slot = 8;
                for(auto & d : directive.allocation)
                {
                    command.instructions.push_back(
                        new CODEGEN::TYPES::MoveInstruction(CODEGEN::TYPES::InstructionSet::MOVE_ADDRESS,
                            param_gs_slot,
                            d.start_pos,
                            d.end_pos - d.start_pos
                        )
                    );
                    param_gs_slot += 8;
                }

                // Call the function
                command.instructions.push_back(
                    new CODEGEN::TYPES::CallInstruction(CODEGEN::TYPES::InstructionSet::CALL, directive.data)
                );
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
    }
    
    // ----------------------------------------------------------
    //
    // ----------------------------------------------------------

    CODEGEN::TYPES::Command Intermediate::build_assignment(std::vector<std::string> & tokens, uint64_t byte_len)
    {
        CODEGEN::TYPES::Command command;

        // Check all tokens for what they represent
        for(auto & token : tokens)
        {
            // Check for a directive
            if(token[0] == '#')
            {
                build_assignment_directive(command, token, byte_len);
            }
            // Check for char || int || double (raw values)
            else if(token[0] == '"' || is_only_number(token) )
            {
                command.instructions.push_back(
                    new CODEGEN::TYPES::RawValueInstruction(CODEGEN::TYPES::InstructionSet::USE_RAW, token)
                );
            }
            else
            {
                // Get the operation token
                command.instructions.push_back(
                    new CODEGEN::TYPES::BaseInstruction(get_operation(token))
                );
            }
        }

        // If its a return statement, we don't want to add a store command
        if(command.instructions.back()->instruction == CODEGEN::TYPES::InstructionSet::RETURN)
        {
            return command;
        }

        CODEGEN::TYPES::InstructionSet final = (byte_len < 8) ? 
            CODEGEN::TYPES::InstructionSet::STORE_BYTE : 
            CODEGEN::TYPES::InstructionSet::STORE_WORD;

        // End of assignment trigger storage of result
        command.instructions.push_back(
            new CODEGEN::TYPES::BaseInstruction(final)
        );
        return command;
    }

    // ----------------------------------------------------------
    // 
    // ----------------------------------------------------------

    CODEGEN::TYPES::InstructionSet Intermediate::get_operation(std::string token)
    {
        if(token == "ADD"   ) { return CODEGEN::TYPES::InstructionSet::ADD;    }
        if(token == "SUB"   ) { return CODEGEN::TYPES::InstructionSet::SUB;    }
        if(token == "MUL"   ) { return CODEGEN::TYPES::InstructionSet::MUL;    }
        if(token == "DIV"   ) { return CODEGEN::TYPES::InstructionSet::DIV;    }
        if(token == "MOD"   ) { return CODEGEN::TYPES::InstructionSet::MOD;    }
        if(token == "POW"   ) { return CODEGEN::TYPES::InstructionSet::POW;    }
        if(token == "LTE"   ) { return CODEGEN::TYPES::InstructionSet::LTE;    }
        if(token == "LT"    ) { return CODEGEN::TYPES::InstructionSet::LT;     }
        if(token == "GTE"   ) { return CODEGEN::TYPES::InstructionSet::GTE;    }
        if(token == "GT"    ) { return CODEGEN::TYPES::InstructionSet::GT;     }
        if(token == "EQ"    ) { return CODEGEN::TYPES::InstructionSet::EQ;     }
        if(token == "NE"    ) { return CODEGEN::TYPES::InstructionSet::NE;     }
        if(token == "LSH"   ) { return CODEGEN::TYPES::InstructionSet::LSH;    }
        if(token == "RSH"   ) { return CODEGEN::TYPES::InstructionSet::RSH;    }
        if(token == "NEGATE") { return CODEGEN::TYPES::InstructionSet::NEGATE; }
        if(token == "OR"    ) { return CODEGEN::TYPES::InstructionSet::OR;     }
        if(token == "AND"   ) { return CODEGEN::TYPES::InstructionSet::AND;    }
        if(token == "BW_OR" ) { return CODEGEN::TYPES::InstructionSet::BW_OR;  }
        if(token == "BW_XOR") { return CODEGEN::TYPES::InstructionSet::BW_XOR; }
        if(token == "BW_AND") { return CODEGEN::TYPES::InstructionSet::BW_AND; }
        if(token == "BW_NOT") { return CODEGEN::TYPES::InstructionSet::BW_NOT; }
        if(token == "RETURN") { return CODEGEN::TYPES::InstructionSet::RETURN; }
        std::cerr << "Developer error : Intermediate::InstructionSet Intermediate::get_integer_operation(std::string token)" << std::endl;
        exit(EXIT_FAILURE);
    }
}