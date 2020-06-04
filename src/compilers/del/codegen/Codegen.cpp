#include "Codegen.hpp"

#include <iostream>

namespace DEL
{

    // ----------------------------------------------------------
    //
    // ----------------------------------------------------------

    Codegen::Codegen(Errors & err, SymbolTable & symbolTable) : error_man(err), 
                                                                symbol_table(symbolTable), 
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

    void Codegen::accept(Assignment &stmt)
    {
        // Analyzer should have checked to ensure that all symbols required to construct statement exist, 
        // and everything is the correct type. Assume analyzer did its job and now generate some code
    }
    
}