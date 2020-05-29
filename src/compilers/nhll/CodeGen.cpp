#include "CodeGen.hpp"
#include <filesystem>
#include <sstream>
#include <iostream>

namespace NHLL
{
    // --------------------------------------------
    //
    // --------------------------------------------

    CodeGen::CodeGen() : state(GenState::IDLE)
    {
    }

    // --------------------------------------------
    //
    // --------------------------------------------
    
    CodeGen::~CodeGen()
    {

    }

    // --------------------------------------------
    //
    // --------------------------------------------

    bool CodeGen::start_function(std::string name, std::vector<FunctionParam> params,  DataPrims return_type)
    {
        // If this happens, its on us. Either the preprocessor got bonked, or a grammar rule is broken
        if(state != GenState::IDLE)
        {
            std::cerr << "CodeGen::Error : Can not build function while CodeGen in state other than IDLE" 
                      <<  ". Current state : " << state_to_string(state) << std::endl;
            return false;
        }

        functions.push_back(
            FunctionRepresentation(name, params, return_type)
        );

        state = GenState::BUILD_FUNCTION;
        return true;
    }

    // --------------------------------------------
    //  Given a variable name attempt to get its information
    //  so the caller can check type/ access/ etc
    // --------------------------------------------

    CodeGen::VariablePollResult CodeGen::check_variable_access(std::string name, Variable & definition)
    {
        // Go through current function's scope variable map and see if 
        // the variable given exists, and 


        return VariablePollResult::NOT_FOUND;
    }

    // --------------------------------------------
    //
    // --------------------------------------------
    
    std::string CodeGen::state_to_string(CodeGen::GenState state) const
    {
        switch(state)
        {
            case CodeGen::GenState::IDLE:           return "IDLE";
            case CodeGen::GenState::BUILD_FUNCTION: return "BUILD_FUNCTION";
            case CodeGen::GenState::END_FUNCTION:   return "END_FUNCTION";
            default:
                std::cerr << "CodeGen::Error : Trying to print an invalid state!" << std::endl;
                exit(EXIT_FAILURE);
                break;
        }
    }
}