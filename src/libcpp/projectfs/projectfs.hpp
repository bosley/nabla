/*
    Given a directory, the Project FS class will determine if the target is an application or 
    library for NHLL. It will then load the project file structure in a manner that allows us to
    easily obtain files and configurations relevant to compiling / interpreting 

*/

#ifndef NABLA_PROJECT_FS_HPP
#define NABLA_PROJECT_FS_HPP

#include "json.hpp"

namespace NABLA
{
    class ProjectFS
    {
    public:

        enum class ProjectType
        {
            UNKNOWN,
            LIBRARY,
            APPLICATION
        };

        enum class LoadResultCodes
        {
            OKAY,
            ERROR_GIVEN_PATH_NOT_DIRECTORY,
            ERROR_FAILED_TO_OPEN_CONFIG,
            ERROR_FAILED_TO_LOAD_CONFIG,
            ERROR_NO_CONFIG_FOUND
        };

        ProjectFS();
        ~ProjectFS();

        LoadResultCodes load(std::string location);

    private:

        bool projectLoaded;
        ProjectType projectType;
    };
}

#endif