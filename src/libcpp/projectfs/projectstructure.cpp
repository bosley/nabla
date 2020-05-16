#include "projectstructure.hpp"

namespace NABLA
{
    // ---------------------------------------------------------------
    // 
    // ---------------------------------------------------------------

    ProjectStructure::ProjectStructure()
    {
        clear();
    }

    // ---------------------------------------------------------------
    // 
    // ---------------------------------------------------------------

    ProjectStructure::~ProjectStructure()
    {
        clear();
    }

    // ---------------------------------------------------------------
    // 
    // ---------------------------------------------------------------

    std::string ProjectStructure::get_author() const 
    { 
        return author;
    }

    // ---------------------------------------------------------------
    // 
    // ---------------------------------------------------------------

    std::string ProjectStructure::get_description() const 
    { 
        return description;
    }

    // ---------------------------------------------------------------
    // 
    // ---------------------------------------------------------------

    ProjectType ProjectStructure::get_projectType() const 
    { 
        return projectType;
    }

    // ---------------------------------------------------------------
    // 
    // ---------------------------------------------------------------

    std::string ProjectStructure::get_entryFile() const 
    { 
        return entryFile;
    }

    // ---------------------------------------------------------------
    // 
    // ---------------------------------------------------------------

    std::vector<std::string> ProjectStructure::get_module_list() const 
    { 
        return module_list;
    }
}