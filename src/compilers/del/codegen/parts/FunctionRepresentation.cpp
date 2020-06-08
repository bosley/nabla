#include "FunctionRepresentation.hpp"
#include "Generators.hpp"
#include <libnabla/endian.hpp>

#  define N_UNUSED(x) (void)x;

/*

    Parameters are passed by address. 

    Local stack setup
                                   Addresses of 
        Word 0          Word 1     Parameters 
     ________________________________________________
    |             |              |   |   |   |   |
    |  GS Offset  |  GS Fn Size  | 0 | . | . | N |
    |_____________|______________|___|___|___|___|___

    GS Offset   - Offset for GS at the time of function loading
    GS Fn Size  - The size that the function will take up for variable storage in memory , used to free the global stack on return

    When a function is called, the call will access the caller's stack 

*/

namespace DEL
{
namespace PARTS
{
    FunctionRepresentation::FunctionRepresentation(std::string name, std::vector<FunctionParam> params) : name(name), 
                                                                                                          params(params), 
                                                                                                          bytes_required(0)
    {
        // Allocate a WORD for each parameter
        //
        for(auto & p : params)
        {
            N_UNUSED(p)
            bytes_required += 8;
        }
    }

    FunctionRepresentation::~FunctionRepresentation()
    {
        instructions.clear();
    }

    std::vector<std::string> FunctionRepresentation::building_complete()
    {
        std::vector<std::string> lines;

        lines.push_back("<" + name + ":\n");

        lines.push_back("\n\t; Save registers for GS expansion\n");
        lines.push_back("\n\tpushw ls r0");
        lines.push_back("\n\tpushw ls r1");
        lines.push_back("\n\tpushw ls r2");
        lines.push_back("\n\tpushw ls r3");
        lines.push_back("\n\tpushw ls r4");
        lines.push_back("\n\tpushw ls r5");
        lines.push_back("\n\tpushw ls r6");
        lines.push_back("\n\tpushw ls r7\n");

        uint8_t additional = (bytes_required % 8);

        if( additional > 0)
        {
            bytes_required = (8 - bytes_required);
        }

        // div bytes required by 8 to see how many words are required. 
        // shift that number into place
        std::vector<std::string> fsize = generate_store_64(8, ENDIAN::conditional_to_le_64(bytes_required/8), "Required space for : " + name);

        lines.insert(lines.end(), fsize.begin(), fsize.end());

        lines.push_back("\n\t; Expand GS to store the current function instance\n");
        lines.push_back("\n\tmov r1 $0\n");
        lines.push_back("\tmov r2 $0\n");
        lines.push_back("\tmov r9 r0 \t; Copy loaded function size \n");
        lines.push_back("\nfunction_alloc_gs:\n");
        lines.push_back("\n\tadd r1 r1 $1\n");
        lines.push_back("\tpushw gs r2\n");
        lines.push_back("\n\tblt r1 r0 function_alloc_gs\n"); // Keep pushing words until we've taken on the stack frame

        lines.push_back("\n\t; Restore registers to their state from when the function was called\n");
        lines.push_back("\n\tpopw r7 ls");
        lines.push_back("\n\tpopw r6 ls");
        lines.push_back("\n\tpopw r5 ls");
        lines.push_back("\n\tpopw r4 ls");
        lines.push_back("\n\tpopw r3 ls");
        lines.push_back("\n\tpopw r2 ls");
        lines.push_back("\n\tpopw r1 ls");
        lines.push_back("\n\tpopw r0 ls\n");

        lines.push_back("\n\t; Retrieve the stack offset from before the function and store\n");

        lines.push_back("\n\tldw r8 $0(gs)");
        lines.push_back("\n\tpushw ls r8 \t; Stack offset");
        lines.push_back("\n\tpushw ls r9 \t; Our size");

        lines.push_back("\n\tadd r8 r8 r9 \t; Add our size to the gs offset");

        lines.push_back("\n\tstw $0(gs) r8 \t; Increates the stack offset\n");

        lines.insert(lines.end(), instructions.begin(), instructions.end());

        lines.push_back("\n>\n");
        return lines;
    }

    void FunctionRepresentation::build_return()
    {
        // The expression of the return should be on the local stack now, so all we have to do is pop it off into r0
        instructions.push_back("\n\t; <<< RETURN >>> \n");

        instructions.push_back("\n\tldw r8 $0(ls) \t; Initial stack offset");
        instructions.push_back("\n\tldw r9 $8(ls) \t; Our size");
        instructions.push_back("\n\tstw $0(gs) r8 \t; Reset stack offset\n");

        instructions.push_back("\n\t; Shrink GS to clean up the current function instance\n");
        instructions.push_back("\n\tmov r1 $0\n");
        instructions.push_back("\tmov r0 r9 \t; Copy loaded function size \n");
        instructions.push_back("\nfunction_dealloc_gs:\n");
        instructions.push_back("\n\tadd r1 r1 $1\n");
        instructions.push_back("\tpopw r7 gs\n");
        instructions.push_back("\n\tblt r1 r0 function_dealloc_gs\n");
        instructions.push_back("\n\t; Get result for return \n");

        instructions.push_back("\tpopw r0 ls \t; Return result\n");
        instructions.push_back("\tret\n");

    }
}
}