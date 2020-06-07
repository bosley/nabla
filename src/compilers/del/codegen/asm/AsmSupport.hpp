#ifndef DEL_ASM_SUPPORT_HPP
#define DEL_ASM_SUPPORT_HPP

#include <map>
#include <vector>
#include <string>

namespace DEL
{
    class AsmSupport
    {
    public:

        enum class Math
        {
            MOD_D = 0x00,
            MOD_I = 0x01,
            POW_D = 0x02,
            POW_I = 0x03
        };

        AsmSupport();
        ~AsmSupport();

        void import_init_start(std::vector<std::string> & destination);
        void import_init_func(std::vector<std::string> & destination);
        void import_math(AsmSupport::Math math_import, std::string & function_name_out, std::vector<std::string> & destination);

    private:

        struct ImportInfo
        {
            bool imported;
            std::string function_name;
            std::string function;
        };

        struct InitImport
        {
            bool start;
            bool func;
        };
        InitImport init_import;

        std::map<Math, ImportInfo> math_imports;
    };
}

#endif