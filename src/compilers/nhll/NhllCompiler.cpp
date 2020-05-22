#include "NhllCompiler.hpp"

#include <iostream>

namespace NABLA
{
namespace COMPILERS
{

    // -----------------------------------------------
    //
    // -----------------------------------------------

    Nhll::Nhll() : parser(*this)
    {

    }

    // -----------------------------------------------
    //
    // -----------------------------------------------

    NABLA::CompiledPackage Nhll::loadSource(std::string source_file)
    {
        CompiledPackage package;

        // Open the file

        // for each line in file 
        //
        //  parser.parseLine(line)

        return package;
    } 

    // -----------------------------------------------
    //
    // -----------------------------------------------

    void Nhll::handle_error(NABLA::CompilerError compiler_error)
    {
        
    }

    // -----------------------------------------------
    //
    // -----------------------------------------------

    void Nhll::process_let_stmt(std::string var, std::any value, VariableType type)
    {
        std::cout << "Process let : " << var << std::endl;
    }

    // -----------------------------------------------
    //
    // -----------------------------------------------

    void Nhll::process_use_stmt(std::string module_path, std::string as)
    {

    }
}
}