#include "FunctionRepresentation.hpp"
#include "Generators.hpp"
#include <libnabla/endian.hpp>
#include <iostream>
#  define N_UNUSED(x) (void)x;

/*

    Parameters are passed by address. 

    Local stack setup
                                    
        Word 0          Word 1     Parameters 
     ________________________________________________
    |             |              |   |   |   |   |
    |  GS Offset  |  GS Fn Size  | 0 | . | . | N |
    |_____________|______________|___|___|___|___|___

    GS Offset   - Offset for GS at the time of function loading
    GS Fn Size  - The size that the function will take up for variable storage in memory , used to free the global stack on return

*/

namespace DEL
{
namespace PARTS
{
    FunctionRepresentation::FunctionRepresentation(std::string name, std::vector<CODEGEN::TYPES::ParamInfo> params) : 
                                                                                                          name(name), 
                                                                                                          params(params), 
                                                                                                          bytes_required(0)
    {
        // Allocate a space for each parameter
        //
        for(auto & p : params)
        {
            uint64_t bytes_for_param = p.end_pos - p.start_pos;

            bytes_required += bytes_for_param;
        }
    }

    FunctionRepresentation::~FunctionRepresentation()
    {
        instructions.clear();
    }

    std::vector<std::string> FunctionRepresentation::building_complete()
    {
        std::vector<std::string> lines;

        lines.push_back("<" + name + ":\n\n");

        lines.push_back("\tldw r8 $0(gs)\t ; Load the current stack offset\n");
        lines.push_back("\tpushw ls r8 \t ; Store it in the local stack\n");

        lines.push_back("\tmov r9 $" + std::to_string(ENDIAN::conditional_to_le_64(bytes_required/8)) + "\t ; Words required for function (" + name + ")\n" );
        lines.push_back("\tpushw ls r9 \t ; Store it in the local stack\n\n");

        lines.push_back("\n\t; Expand GS to store the current function instance\n");
        lines.push_back("\n\tmov r1 $0\n");
        lines.push_back("\tmov r2 $0\n");
        lines.push_back("\nfunction_alloc_gs:\n");
        lines.push_back("\n\tadd r1 r1 $1\n");
        lines.push_back("\tpushw gs r2\n");
        lines.push_back("\n\tblt r1 r9 function_alloc_gs\n"); // Keep pushing words until we've taken on the stack frame

        lines.push_back("\n\tadd r8 r8 r9 \t; Add our size to the gs offset");
        lines.push_back("\n\tstw $0(gs) r8 \t; Increates the stack offset\n");

        lines.push_back("\n; --- Copy Parameters into function space --- \n\n");

        for(auto & p : params)
        {
         //   lines.push_back("\tldw r0 $" + std::to_string(p.param_gs_index) + "(gs)\t ; Move address of parameter\n");
         //   lines.push_back("\tldw r2 $0(ls)\t ; Load this functions stack offset\n");

            std::cerr << ">>>>>>>>> WARNING <<<<<<<<<<< : We need to assemble local representation of "
                      << " parameters passed in via the addresses stored in the global stack - FunctionRepresentation.cpp"
                      << std::endl;

            /*
                    Right here we have p.gs_param_start which will tell us what index of the GS an incomming parameter's 
                    current address is at (well, once call is written) 

                    We need to take p.start_pos (our copy of the parameter's new address) and add it to the local
                    stack's reference to the gs start index $0(ls) so we can ldw the address stored in gs for the parameter
                    into a register and then stw that data in our stack frame where the memory manager assigned it

                    To do this I'd like to utilize the 2-Mov method of loading addresses into a register
                     - That was just written. But im tired and will do this tomorrow
            */

           std::cout << "Current address of parameter to be passed in   : " << (int)p.param_gs_index << std::endl;
           std::cout << "Our address for it sans the stack frame offset : " << p.start_pos << std::endl;
        }
        

        lines.insert(lines.end(), instructions.begin(), instructions.end());

        lines.push_back("\n>\n");
        return lines;
    }

    void FunctionRepresentation::build_return(bool return_item)
    {
        // The expression of the return should be on the local stack now, so all we have to do is pop it off into r0
        instructions.push_back("\n\t; <<< RETURN >>> \n");

        instructions.push_back("\n\tldw r8 $0(ls) \t; Initial stack offset");
        instructions.push_back("\n\tldw r9 $8(ls) \t; Our size");
        instructions.push_back("\n\tstw $0(gs) r8 \t; Reset stack offset\n");

        instructions.push_back("\n\t; Shrink GS to clean up the current function instance\n");
        instructions.push_back("\n\tmov r1 $0\n");
        instructions.push_back("\nfunction_dealloc_gs:\n");
        instructions.push_back("\n\tadd r1 r1 $1\n");
        instructions.push_back("\tpopw r7 gs\n");
        instructions.push_back("\n\tblt r1 r9 function_dealloc_gs\n");

        if(return_item)
        {
            instructions.push_back("\n\t; Get result for return \n");
            instructions.push_back("\tpopw r0 ls \t; Return result\n");
        }
        
        instructions.push_back("\tret\n");
    }
}
}