#include "CodeGen.hpp"
#include <filesystem>
#include <sstream>
#include <iostream>

namespace NHLL
{
    // --------------------------------------------
    //
    // --------------------------------------------

    CodeGen::CodeGen(NABLA::LibManifest & lib_manifest) : local_driver(nullptr), project_fs(nullptr), lib_manifest(lib_manifest)
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

    // --------------------------------------------
    //
    // --------------------------------------------
    
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

    // --------------------------------------------
    //
    // --------------------------------------------
    
    bool CodeGen::load_module(std::string module, std::string alias)
    {
        std::vector<std::string> module_parts;
     
        // Build a module_parts list by splitting by '.'
        {
            std::string part;
            std::istringstream ss(module);
            while(std::getline(ss, part, '.'))
            {
                module_parts.push_back(part);
            }
        }

        std::cout << "Load module : " << module_parts[0] << std::endl;

        bool found_module = false;
        NABLA::ProjectStructure target_module;

        //  If the local projectfs has been given, then search local project for the module
        //
        if(nullptr != project_fs)
        {
            NABLA::ProjectStructure ps = project_fs->get_project_structure();

            std::vector<std::string> module_list = ps.get_module_list();

            for(auto & mod : module_list)
            {
                auto path = std::filesystem::path(mod);
                std::string module_name = path.parent_path().filename();

                if(module_name ==  module_parts[0])
                {
                    target_module = ps;
                    found_module = true;
                }
            }
        }


        //  If the module wasn't found in the local project, search installed libs
        //
        if(!found_module)
        {
            //  If it isn't found by now we expect at least 2 parts to identify the module (stdlib. something)
            //
            if(module_parts.size() < 2)
            {
                std::cerr << "Unable to find module \"" << module_parts[0] << "\"." << std::endl;
                return false;
            }

            bool found = false;
            NABLA::ProjectFS fps = lib_manifest.get_library(module_parts[0], module_parts[1], found);

            if(!found)
            {
                std::cerr << "Unable to find module \"" << module << "\"." << std::endl;
                return false;
            }

            target_module = fps.get_project_structure();
            found_module = true;
        }

        // Sanity check
        //
        if(!found_module)
        {
            std::cerr << "Unable to find module \"" << module << "\"." << std::endl;
            return false;
        }

        std::cout << "Module found! Entry file : " << target_module.get_entryFile() << std::endl;

#warning WE NEED TO ENSURE IT HASN'T BEEN LOADED ALREADY!!

        local_driver = new NHLL_Driver(*this);

#warning Need to add 'public' or 'export' to language before we can continue here. Export would be a better idea for lib.n and mod.n so we know what specifically to load

        local_driver->parse(target_module.get_entryFile().c_str());

        delete local_driver;
        /*

            Before we can implement the load module we need to implement the library manifest.


            Steps : 


           X LibManifest will populate stdlib and external libs into a map of project_fs'

           X this method here will assume the given module is a 'local' module first, and if it isn't found, 
           X then it asks lib manifest if it has the given module. 

           X If no module is found, we error out and die. 

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