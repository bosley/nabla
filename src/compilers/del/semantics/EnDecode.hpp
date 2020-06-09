#ifndef DEL_ENDECODE_HPP
#define DEL_ENDECODE_HPP

#include <string>
#include "Ast.hpp"
#include "Memory.hpp"

/*
    Analyzer needs help encoding complex data types in a way that the intermediate layer can make sense of them
    Since I opted for a post-fix expression thing the way I did, this is required. When a node is found to be a complex 
    object, and not a simple operation, we need to use EnDecode to put help build the expression. 

    Once the expression is built it is scanned by the Intermediate object and turned into instructions for the code generator, 
    so this will play the role of decoding the complex types when its time to generate code generator instructions. 

*/

namespace DEL
{
    class EnDecode
    {
    public:


        EnDecode(Memory & memory_man);
        ~EnDecode();


        std::string encode(Call * function_call);


    private:
        Memory & memory_man;
    };
}


#endif