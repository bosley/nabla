
#include <iostream>
#include <string>
#include <vector>

#include "BinExec.hpp"
#include "CliInterpreter.hpp"

/*
    LLL - Low level language    (LLL Binary is a binary generated from raw LLL through solace)
    HLL - High level language   (HLL Binary is a binary generated from nabla HLL compiler)
*/

namespace
{
    struct Args
    {
        std::string short_arg;
        std::string long_arg;
        std::string description;
    };

    std::vector<Args> NablaArguments;
}

// --------------------------------------------
//  Execute a binary 
// --------------------------------------------
    
int  handle_bin_exec(std::string file)
{
    NABLA::BinExec exec;
    if(!exec.loadBinaryFromFile(file))
    {
        // Error messages generated by BinExec
        return 1;
    }

    if(!exec.execute())
    {
        // Do nothing for now. In the future, an option might exist where we pipe the binary to HARP
        return 1;
    }

    return 0;
}

// --------------------------------------------
// Compile Nabla HLL
// --------------------------------------------
    
int handle_compilation(std::string file)
{
    std::cout << "Nabla compiler has not yet been developed" << std::endl;
    return 1;
}

// --------------------------------------------
// Interpret Nabla HLL cli
// --------------------------------------------

int handle_interpretation_cli()
{
    std::cout << " ∇ Nabla ∇ " << NABLA_VERSION_INFO     << std::endl 
              << "Platform: "  << TARGET_PLATFORM_STRING << std::endl
              << "------------------------------------" << std::endl; 

    NABLA::CliInterpreter cliInterpreter;
    return cliInterpreter.begin();
}

// --------------------------------------------
// Interpret Nabla HLL project
// --------------------------------------------

int handle_interpretation_project(std::string project_dir)
{
    std::cout << "Project interpretation has not yet developed" << std::endl;
    return 1;
}

// --------------------------------------------
// Show help
// --------------------------------------------
    
void show_help()
{
    std::cout << " ∇ Nabla ∇ | Help" << std::endl 
              << "----------------------------------------------" 
              << std::endl
              << "Short\tLong\t\tDescription" << std::endl
              << "----------------------------------------------"
              << std::endl;

    for(auto & na : NablaArguments)
    {
        std::cout << na.short_arg << "\t" << na.long_arg << "\t" << na.description << std::endl;
    }

    std::cout << "----------------------------------------------"
              << std::endl 
              << "Given no commands, Nabla will enter into interpreter."
              << std::endl 
              << "Given a single file, Nabla will assume that it is a compiled HLL project and attempt to execute it."
              << std::endl;
}

// --------------------------------------------
// Show version
// --------------------------------------------
    
void show_version()
{
    std::cout << " ∇ Nabla ∇ | Version and build information"       << std::endl
              << "-------------------------------------------"      << std::endl 
              << "Nabla Version  : " << NABLA_VERSION_INFO          << std::endl
              << "Short Build ID : " << NABLA_BUILD_ID_SHORT        << std::endl
              << "Compiled       : " << NABLA_COMPILATION_TIMESTAMP << std::endl
              << "-------------------------------------------"      << std::endl;
    return;
}

// --------------------------------------------
// Entry
// --------------------------------------------
    
int main(int argc, char ** argv)
{
    if(argc == 1)
    {
        return handle_interpretation_cli();
    }

    NablaArguments = {

        { "-h", "--nabla-help", "Display help message."},
        { "-v", "--version",    "Display the version of Nabla." },
        { "-i", "--interpret",  "Interpret a Nabla HLL project."},
        { "-c", "--compile",    "Compile a nabla HLL project."}
    };
    
    std::vector<std::string> args(argv, argv + argc);

    for(int i = 0; i < argc; i++)
    {
        //  Help
        //
        if(args[i] == "-h" || args[i] == "--nabla-help")
        {
            show_help();
            return 0;
        }

        // Version info
        //
        if(args[i] == "-v" || args[i] == "--version")
        {
            show_version();
            return 0;
        }

        // Interpret a nabla HLL project (run without compile)
        //
        if(args[i] == "-i" || args[i] == "--interpret")
        {
            if(i == argc - 1)
            {
                std::cout << "Error: Project directory not given" << std::endl;
                return 1;
            }

            return handle_interpretation_project(args[i+1]);
        }
        
        // Compile a nabla HLL project
        //
        if(args[i] == "-c" || args[i] == "--compile")
        {
            if(i == argc - 1)
            {
                std::cout << "Error: Project directory not given" << std::endl;
                return 1;
            }

            return handle_compilation(args[i+1]);
        }
    }

    // No arguments handled, but there is at least one argument so try to execute it as a HLL project

    return handle_bin_exec(args[1]);
}