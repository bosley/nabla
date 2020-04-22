#include "Analyzer.hpp"
#include <iostream>

extern "C" 
{
    #include <stdio.h>
    #include "vm.h"
    #include "binloader.h"
}

namespace HARP
{
    // --------------------------------------------
    //
    // --------------------------------------------

    Analyzer::Analyzer()
    {
        // Create the virutal machine
        vm = vm_new();

        // Ensure the VM is like.. there
        if(vm == NULL)
        {
            std::cerr << "Analyzer : New VM was instantiated as NULL" << std::endl;
            exit(EXIT_FAILURE);
        }
    }

    // --------------------------------------------
    //
    // --------------------------------------------
    
    bool Analyzer::loadBin(std::string file)
    {
        /*
                This loads a vm so we can extract the information without having to re-write all of
                the VM load code for no good reason.
        */
        FILE * file_in = fopen(file.c_str(), "rb");

        if(file_in == NULL)
        {
            std::cerr << "Analyzer : Unable to open file " << file << std::endl;
            return false;
        }
        
        // Hand the file and vm over to be populated
        switch(bin_load_vm(file_in, vm))
        {
            case VM_LOAD_ERROR_NULL_VM:
                std::cerr << "Analyzer : The VM given was null" << std::endl;
                return false;

            case VM_LOAD_ERROR_FILE_OPEN:    
                std::cerr << "Analyzer : There was an error opening the bytecode file" << std::endl;
                return false;

            case VM_LOAD_ERROR_FAILED_TO_LOAD_CONSTANTS:    
                std::cerr << "Analyzer : There was an error loading constants from the bytecode file" << std::endl;
                return false;

            case VM_LOAD_ERROR_FAILED_TO_LOAD_FUCNTION:
                std::cerr << "Analyzer : There was an error loading function from the bytecode file" << std::endl;
                return false;

            case VM_LOAD_ERROR_UNHANDLED_INSTRUCTION:   
                std::cerr << "Analyzer : The loader came across something it didn't understand and threw a fit" << std::endl;
                return false;

            case VM_LOAD_ERROR_ALREADY_LOADED:
                std::cerr << "Analyzer : The VM has already been loaded" << std::endl;
                return false;

            case VM_LOAD_ERROR_EOB_NOT_FOUND:
                std::cerr << "Analyzer : Binary file didn't give a binary EOF instruction" << std::endl;
                return false;

            default:
                return true;
        }
        
        fclose(file_in);
        return true;
    }
}