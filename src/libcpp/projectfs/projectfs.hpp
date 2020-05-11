/*
    Given a directory, the Project FS class will determine if the target is an application or 
    library for NHLL. It will then load the project file structure in a manner that allows us to
    easily obtain files and configurations relevant to compiling / interpreting 

*/

#ifndef NABLA_PROJECT_FS_HPP
#define NABLA_PROJECT_FS_HPP

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

        ProjectFS();
        ~ProjectFS();

    private:

        bool projectLoaded;
        ProjectType projectType;
    };
}

#endif