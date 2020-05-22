#ifndef NABLA_COMPILE_ERROR_HPP
#define NABLA_COMPILE_ERROR_HPP

#include <stdint.h>
#include <string>
#include <vector>

namespace NABLA
{
    //!\brief A compile error!
    struct CompilerError
    {
        std::string what;                       // Error Message
        uint64_t line_number;                   // Line number
        uint16_t line_index;                    // Index in line
        std::vector<std::string> suggestions;   // Suggestions, if any
    };
}


#endif 