#ifndef NABLA_COMPILER_IF_HPP
#define NABLA_COMPILER_IF_HPP

#include "CompiledPackage.hpp"

namespace NABLA
{
    //! \brief A compiler interface so the Nabla application can easily integrate to any developed compiler
    class CompilerIf
    {
    public:
        //! \brief Pass a string to indicate where to get the source from
        //! \retval 
        virtual CompiledPackage loadSource(std::string source) = 0;
    };
}

#endif 