#include "CompiledPackage.hpp"

namespace NABLA
{
    // ---------------------------------------- PUBLIC -----------------------------------------

    // ---------------------------------------------------
    //
    // ---------------------------------------------------

    CompiledPackage::CompiledPackage() : hasError(false)
    {

    }

    // ---------------------------------------------------
    //
    // ---------------------------------------------------
    
    CompiledPackage::~CompiledPackage()
    {
        clearByteCode();
    }

    // ---------------------------------------------------
    //
    // ---------------------------------------------------

    bool CompiledPackage::isErrorPresent() const
    {
        return hasError;
    }

    // ---------------------------------------------------
    //
    // ---------------------------------------------------
    
    std::vector<uint8_t> CompiledPackage::getByteCode() const
    {
        return resulting_bytecode;
    }

    // ---------------------------------------------------
    //  
    // ---------------------------------------------------
    
    void CompiledPackage::addError(CompilerError error)
    {
        hasError = true;
        error_objects.push_back(error);
    }

    // --------------------------------------- PROTECTED ---------------------------------------

    // ---------------------------------------------------
    //
    // ---------------------------------------------------
    
    void CompiledPackage::setByteCode(std::vector<uint8_t> bc)
    {
        clearByteCode();
        resulting_bytecode = bc;
    }

    // ---------------------------------------------------
    //
    // ---------------------------------------------------
    
    void CompiledPackage::clearByteCode()
    {
        resulting_bytecode.clear();
    }

}