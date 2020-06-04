#include "SymbolTable.hpp"
#include <iostream>
namespace DEL
{

    // ----------------------------------------------------------
    //
    // ----------------------------------------------------------

    SymbolTable::SymbolTable()
    {

    }

    // ----------------------------------------------------------
    //
    // ----------------------------------------------------------

    SymbolTable::~SymbolTable()
    {

    }

    // ----------------------------------------------------------
    //
    // ----------------------------------------------------------

    void SymbolTable::new_context(std::string name, bool remove_previous)
    {
        if(remove_previous)
        {
            // Don't allow the removal of the global context
            if(contexts.size() > 1)
            {
                std::cout << "If you're seeing this, its the first time you've attempted to delete a context" << std::endl;

                Context * ciq = contexts.back();
                delete ciq;
                contexts.pop_back();
            }
        }

        contexts.push_back(new Context(name));
    }

    // ----------------------------------------------------------
    //
    // ----------------------------------------------------------

    bool SymbolTable::does_symbol_exist(std::string symbol, bool show_if_found)
    {
        for(auto & c : contexts)
        {
            if(c != nullptr)
            {
                if(c->symbol_map.find(symbol) != c->symbol_map.end())
                {
                    if(show_if_found)
                    {
                        std::cout << "\"" << symbol 
                                  << "\" found in context \"" 
                                  << c->context_name 
                                  << "\" defined as : " 
                                  << ValType_to_string(c->symbol_map[symbol])
                                  << std::endl;
                    }
                    return true;
                }
            }
        }
        return false;
    }

    // ----------------------------------------------------------
    //
    // ----------------------------------------------------------

    bool SymbolTable::does_context_exist(std::string context)
    {
        for(auto & c : contexts)
        {
            if(c != nullptr)
            {
                if(c->context_name == context)
                {
                    return true;
                }
            }
        }
        return false;
    }
    // ----------------------------------------------------------
    //
    // ----------------------------------------------------------

    bool SymbolTable::is_existing_symbol_of_type(std::string symbol, ValType type)
    {
        for(auto & c : contexts)
        {
            if(c != nullptr)
            {
                if(c->symbol_map.find(symbol) != c->symbol_map.end())
                {
                    if(c->symbol_map[symbol] == type)
                    {
                        return true;
                    }
                    else
                    {
                        return false;
                    }
                }
            }
        }
        return false;
    }

    // ----------------------------------------------------------
    //
    // ----------------------------------------------------------

    void SymbolTable::add_symbol(std::string symbol, DEL::ValType type)
    {
        contexts.back()->symbol_map[symbol] = type;
    }

    // ----------------------------------------------------------
    //
    // ----------------------------------------------------------

    DEL::ValType SymbolTable::get_value_type(std::string symbol)
    {
        for(auto & c : contexts)
        {
            if(c != nullptr)
            {
                if(c->symbol_map.find(symbol) != c->symbol_map.end())
                {
                    return c->symbol_map[symbol];
                }
            }
        }
        return ValType::NONE;
    }

    // ----------------------------------------------------------
    //
    // ----------------------------------------------------------

    std::string SymbolTable::get_current_context_name() const
    {
        return contexts.back()->context_name;
    }
}