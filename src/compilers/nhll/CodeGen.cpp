#include "CodeGen.hpp"

#include <iostream>

namespace NHLL
{
    namespace
    {
        bool load_project(std::string project_dir, NABLA::ProjectFS & project)
        {
            switch(project.load(project_dir))
            {
                case NABLA::ProjectFS::LoadResultCodes::OKAY:
                    std::cout << "[" << project_dir << "] loaded!" << std::endl;
                    return true;

                case NABLA::ProjectFS::LoadResultCodes::ERROR_GIVEN_PATH_NOT_DIRECTORY:
                    std::cerr << "[" << project_dir << "] Is not a directory" << std::endl;
                    return false;

                case NABLA::ProjectFS::LoadResultCodes::ERROR_FAILED_TO_OPEN_CONFIG:
                    std::cerr << "Unable to open config.json" << std::endl;
                    return false;

                case NABLA::ProjectFS::LoadResultCodes::ERROR_FAILED_TO_LOAD_CONFIG:
                    std::cerr << "Unable to load config.json" << std::endl;
                    return false;
            }
            return false;
        }
    }

    CodeGen::CodeGen(NABLA::LibManifest & lib_manifest) : local_driver(nullptr), project_fs(nullptr), lib_manifest(lib_manifest)
    {
        current_path.push_back("project");
    }

    CodeGen::~CodeGen()
    {

    }

    void CodeGen::set_project(NABLA::ProjectFS & project)
    {
        project_fs = &project;
    }

    bool CodeGen::gen_use_statement(NHLL::UseStmt statement)
    {
        // Attempt to find the module that is requested
        // If the module isn't loaded already, load it and alias it if requested

        std::cout << "CodeGen :: Use Statement : " << statement.module << " as " << statement.as_name << std::endl;

        if(!load_module(statement.module, statement.as_name))
        {
            return false;
        }

        return true;
    }



    bool CodeGen::load_module(std::string module, std::string alias)
    {

        /*

            Before we can implement the load module we need to implement the library manifest.


            Steps : 


            LibManifest will populate stdlib and external libs into a map of project_fs'

            this method here will assume the given module is a 'local' module first, and if it isn't found, 
            then it asks lib manifest if it has the given module. 

            If no module is found, we error out and die. 

            If the module is found, we are then left with a ProjectFS, either from the main projectFS given
            by the framework, or by the manifest.

            At this point we instantiate the local_driver pointer and tell it to parse the entry file given 
            "*this" as the code gen. It will call into us to generate the given file. 

            Once this is completed, we take the given alias and assign that alias to the loaded 
            but yet to be realized FunctionRepresentation and Module structure storage so if its looked for later,
            we can validate its existence and calls into it. 

        NABLA::ProjectFS * current_module;

        if(nullptr != project_fs)
        {
            current_module = project_fs;
        }
        else
        {
        }



        local_driver = new NHLL_Driver(*this);

 

        // Use local driver to parse 'external' uses

        delete local_driver;

        */
       std::cerr << "'use' not yet completed. Need to finalize manifest first" << std::endl;
       return false;
    }
}