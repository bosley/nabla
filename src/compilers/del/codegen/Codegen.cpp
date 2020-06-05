#include "Codegen.hpp"

#include <iostream>

namespace DEL
{

    // ----------------------------------------------------------
    //
    // ----------------------------------------------------------

    Codegen::Codegen(Errors & err, SymbolTable & symbolTable, Memory & memory) : 
                                                                error_man(err), 
                                                                symbol_table(symbolTable), 
                                                                memory_man(memory),
                                                                building_function(false)
    {

    }

    // ----------------------------------------------------------
    //
    // ----------------------------------------------------------

    Codegen::~Codegen()
    {

    }

    // ----------------------------------------------------------
    //
    // ----------------------------------------------------------

    void Codegen::begin_function(std::string name, std::vector<FunctionParam> params)
    {
        if(building_function)
        {
            std::cerr << "Internal Error >>> Codegen asked to start function while building function "
                      << "grammar should have prevented this!!!" << std::endl;
            exit(EXIT_FAILURE);
        }

        building_function = true;

        std::cout << "Codegen >>> begin_function : " << name << std::endl;
    }

    // ----------------------------------------------------------
    //
    // ----------------------------------------------------------

    void Codegen::end_function()
    {
        if(!building_function)
        {
            std::cerr << "Internal Error >>> Codegen asked to end function while not building function "
                      << "grammar should have prevented this!!!" << std::endl;
            exit(EXIT_FAILURE);
        }

        building_function = false;

        std::cout << "Codegen >>> end_function()" << std::endl;
    }

    // ----------------------------------------------------------
    //
    // ----------------------------------------------------------

    void Codegen::assignment(Intermediate::Assignment assignment)
    {
        std::cout << "CODEGEN : Assignement!" << std::endl;

        /*
            assignment.id                    -> The name of the thing we are assigning for comments
            assignemnt.memory_info           -> Where we need to store the thing
            assignment.assignment_classifier -> How to treat the given data (int, char, real)
            assignment.instructions          -> What to do to the data in RPN form
        */
    }
}