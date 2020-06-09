#ifndef DEL_FUNCTION_REPRESENTATION
#define DEL_FUNCTION_REPRESENTATION

#include "Types.hpp"
#include "CodegenTypes.hpp"
#include <string>
#include <vector>

namespace DEL
{
namespace PARTS
{
    class FunctionRepresentation
    {
    public:
        FunctionRepresentation(std::string name, std::vector<CODEGEN::TYPES::ParamInfo> params);

        ~FunctionRepresentation();

        std::vector<std::string>  building_complete();

        void build_return(bool return_item=true);

        std::string name;
        std::vector<CODEGEN::TYPES::ParamInfo> params;
        std::vector<std::string> instructions;

        uint64_t bytes_required;
    };
}
}


#endif 