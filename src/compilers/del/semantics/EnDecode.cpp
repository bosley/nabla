#include "EnDecode.hpp"

#include <libnabla/json.hpp>

namespace DEL
{
    namespace
    {
        struct EncodedCallParam
        {

        };
        
        // Nobody outside needs to know 
        struct EncodedCall
        {
            
        };
    }

    // ----------------------------------------------------------
    //
    // ----------------------------------------------------------

    EnDecode::EnDecode(Memory & memory_man) : memory_man(memory_man)
    {

    }
       
    // ----------------------------------------------------------
    //
    // ----------------------------------------------------------

    EnDecode::~EnDecode()
    {

    }

    // ----------------------------------------------------------
    //
    // ----------------------------------------------------------

    std::string EnDecode::encode(Call * function_call)
    {


        return "#CALL:NOT:DONE>" + function_call->name;
    }
}