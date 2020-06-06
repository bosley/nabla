#ifndef DEL_SYMBOL_TABLE_HPP
#define DEL_SYMBOL_TABLE_HPP

#include <vector>
#include <map>
#include <string>
#include "Ast.hpp"
#include "Memory.hpp"
#include "Errors.hpp"

namespace DEL
{
    class Codegen;

    class SymbolTable
    {
    public:
        SymbolTable(Errors & error_man, Memory & memory_man);
        ~SymbolTable();

        void new_context(std::string name, bool remove_previous=false);

        bool does_symbol_exist(std::string symbol, bool show_if_found=false);

        bool does_context_exist(std::string context);

        bool is_existing_symbol_of_type(std::string symbol, ValType type);

        void add_symbol(std::string symbol, DEL::ValType, uint64_t memory=0);

        void clear_existing_context(std::string context);

        DEL::ValType get_value_type(std::string symbol);

        std::string get_current_context_name() const;

        friend Codegen;

    private:
    

        Errors & error_man;
        Memory & memory_man;
        
        bool is_locked;

        void lock();

        class Context
        {
        public:
            Context(std::string name) : context_name(name){}
            std::string context_name;
            std::map< std::string, DEL::ValType > symbol_map; 
        };

        std::vector<Context * > contexts; 
    };
}

#endif