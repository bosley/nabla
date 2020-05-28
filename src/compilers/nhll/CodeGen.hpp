#ifndef NABLA_CODE_GEN_HPP
#define NABLA_CODE_GEN_HPP

#include <string>
#include <vector>
#include <map>

#include "nhll.hpp"
#include "nhll_driver.hpp"

#include "LibManifest.hpp"

#include <libnabla/projectfs.hpp>
#include <libnabla/scopetree.hpp>

namespace NHLL
{
    //! \brief Code generator - Generated byte code
    class CodeGen
    {
    public:
        //! \brief Construct a CodeGen
        //! \param lib_manifest A loaded LibManifest object for pulling local libraries
        CodeGen(NABLA::LibManifest & lib_manifest);

        //! \brief Deconstruct a CodeGen
        ~CodeGen();

        //! \brief Set the current project (one asked to gen code for)
        //! \param project The project that we are generating code for
        void set_project(NABLA::ProjectFS & project);

        //! \brief Generate a use statement
        //! \param statement The object representing a use statement 
        bool gen_use_statement(NHLL::UseStmt statement);

    private:

        // Used to parse / externals requested from code given
        // by user driver
        NHLL::NHLL_Driver * local_driver;

        NABLA::ProjectFS * project_fs;

        NABLA::LibManifest & lib_manifest;

        NABLA::ScopeTree scope_tree;
        
        std::vector<std::string> current_path;

        struct FunctionRepresentation
        {
            std::string name;
            std::vector<NHLL::FunctionParam> paramters;
            NHLL::DataPrims return_type;
            std::vector<uint8_t> bytes;
        };

        struct Module
        {
            std::string name;
            std::vector<FunctionRepresentation> functions;
        };


        std::vector<Module> modules;

        bool load_module(std::string module, std::string alias);
    };
}

#endif