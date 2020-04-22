#ifndef HARP_COMPOSER_HPP
#define HARP_COMPOSER_HPP

#include <cstdint>
#include <string>
#include <vector>

extern "C" 
{
    #include "VmCommon.h"
    #include "VmInstructions.h"
}

typedef struct VM * NablaVirtualMachine;

namespace HARP
{
    //! \class Analyzer
    //! \brief Loads a vm from whatever bin is given and uses that vm to access information about the code
    class Analyzer
    {
    public:
        //! \brief Construct an analyzer
        Analyzer();

        //! \brief Load a binary
        //! \param file The path to the binary
        //! \retval true The file was loaded 
        //! \retval false The file failed to load
        bool loadBin(std::string file);

        //! \brief Get the contents of the global stack
        //! \returns The contents of the global stack
        //! \pre Binary should be loaded via loadBin
        //! \post Vm global stack will be emptied 
        std::vector<int64_t> getGlobalStack();
        
    private:
        bool loaded;
        NablaVirtualMachine vm;
    };
}


#endif