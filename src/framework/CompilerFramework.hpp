#ifndef NABLA_COMPILER_FRAMEWORK_HPP
#define NABLA_COMPILER_FRAMEWORK_HPP

#include <string>

namespace NABLA
{
    class CompilerFramework
    {
    public:
        CompilerFramework();
        ~CompilerFramework();

        void compile_project(std::string project_directory);

    };
}

#endif