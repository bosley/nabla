#include "CompilerFramework.hpp"

#include <iostream>

#include "CodeGen.hpp"
#include "nhll.hpp"
#include "nhll_driver.hpp"
namespace NABLA
{
    // ------------------------------------------------
    //
    // ------------------------------------------------
    
    CompilerFramework::CompilerFramework(NABLA::LibManifest & lm) : lib_manifest(lm)
    {

    }

    // ------------------------------------------------
    //
    // ------------------------------------------------
    
    CompilerFramework::~CompilerFramework()
    {

    }

    // ------------------------------------------------
    //
    // ------------------------------------------------
    
    int CompilerFramework::compile_project(NABLA::ProjectFS & project)
    {
        std::cout << "Compile framework not yet setup" << std::endl;

        NHLL::CodeGen code_generator(lib_manifest);

        code_generator.set_project(project);

        NHLL::NHLL_Driver driver(code_generator);

        //
        //  This will all change once the compiler is completed
        //
        NABLA::ProjectStructure ps = project.get_project_structure();
        
        driver.parse( ps.get_entryFile().c_str() );

        //driver.parse( theFile.nhll );

        //driver.print( std::cout ) << "\n";
        return 1;
    }
}