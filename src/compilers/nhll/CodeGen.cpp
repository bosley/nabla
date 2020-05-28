#include "CodeGen.hpp"
#include <filesystem>
#include <sstream>
#include <iostream>

namespace NHLL
{
    // --------------------------------------------
    //
    // --------------------------------------------

    CodeGen::CodeGen() : local_driver(nullptr), project_fs(nullptr)
    {
        current_path.push_back("project");
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
    
    void CodeGen::set_project(NABLA::ProjectFS & project)
    {
        project_fs = &project;
    }

}