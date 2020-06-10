#ifndef DEL_GENERATOR_HPP
#define DEL_GENERATOR_HPP

#include "AsmSupport.hpp"
#include "CodegenTypes.hpp"

#include "FunctionRepresentation.hpp"

#include <vector>
#include <string>

namespace DEL
{
    class Generator
    {
    public:
        Generator();
        ~Generator();

        void complete_code_generation(std::vector<std::string> & output);

        void add_instructions(std::vector<std::string> instructions);

        void include_builtin_math_pow(CODEGEN::TYPES::DataClassification classification, std::string & out_function_name);
        void include_builtin_math_mod(CODEGEN::TYPES::DataClassification classification, std::string & out_function_name);

        std::vector<std::string> load_64_into_r0(uint64_t le_64, std::string comment);

    private:
        AsmSupport asm_support;

        std::vector<std::string> built_ins_triggered;
        std::vector<std::string> program_instructions;
    };
}

#endif