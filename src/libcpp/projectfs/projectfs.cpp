#include "projectfs.hpp"

#include <iostream>
#include <fstream>
#include <filesystem>

namespace NABLA
{
    ProjectFS::ProjectFS() : projectLoaded(false),
                             projectType(ProjectFS::ProjectType::UNKNOWN)
    {

    }

    ProjectFS::~ProjectFS()
    {
        
    }
    
    ProjectFS::LoadResultCodes ProjectFS::load(std::string location)
    {
        projectLoaded = false;
        projectType = ProjectType::UNKNOWN;
        
        // Ensure what they gave us is a directory
        if(!std::filesystem::is_directory(std::filesystem::status(location)))
        {
            return LoadResultCodes::ERROR_GIVEN_PATH_NOT_DIRECTORY;
        }

        // Add config.json to the path .. using /= ?? Okay..
        auto path = std::filesystem::path(location);
        path /= "config.json";

        // Open the json file
        std::ifstream ifs(path);
        if(!ifs.is_open()) 
        {
            return LoadResultCodes::ERROR_FAILED_TO_OPEN_CONFIG;
        }

        // Attempt to load the json - We can add particular catches later
        njson::json configuration_file;
        try
        {
            ifs >> configuration_file;
        }
        catch(...)
        {
            return LoadResultCodes::ERROR_FAILED_TO_LOAD_CONFIG;
        }


        return LoadResultCodes::OKAY;
    }
}