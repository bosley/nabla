#include "SymbolTable.hpp"
#include <iostream>
namespace DEL
{

    // ----------------------------------------------------------
    //
    // ----------------------------------------------------------

    SymbolTable::SymbolTable(Errors & error_man, Memory & mm) : error_man(error_man),
                                                                memory_man(mm)
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

    void SymbolTable::clear_existing_context(std::string context)
    {
        for(auto & c : contexts)
        {
            if(c != nullptr)
            {
                if(c->context_name == context)
                {
                    c->symbol_map.clear();
                    return;
                }
            }
        }
        error_man.report_custom("SymbolTable", "Developer error: Asked to clear existing context, but that context did not exist", true);
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

    void SymbolTable::add_symbol(std::string symbol, DEL::ValType type, uint64_t memory)
    {
        contexts.back()->symbol_map[symbol] = type;

        // A stop-gap to ensure we know why things break as stuff is expanded
        if(memory > 0 && memory != 8)
        {
            std::cerr << "You just attempted to allocate \"" << memory << "\" bytes of memory "
                      << "indicating that more complex types are being written. You're seeing this "
                      << "because codegen is not yet able to handle non-word aligned bytes. " 
                      << "This now needs to be supported. Thank you!";
            error_man.report_custom("SymbolTable", "Requires further development", true);
        }

        // Depending on the val_type we need to allocate some memory
        uint64_t mem_request = 0;
        switch(type)
        {
        case ValType::INTEGER:       mem_request = (memory == 0) ? 8 : memory; break;
        case ValType::REAL:          mem_request = (memory == 0) ? 8 : memory; break;
        case ValType::CHAR:          mem_request = (memory == 0) ? 8 : memory; break;
        case ValType::FUNCTION:      error_man.report_custom("SymbolTable", " FUNCTION given to symbol table", true);
        case ValType::REQ_CHECK:     error_man.report_custom("SymbolTable", " REQ CHECK given to symbol table", true);
        case ValType::NONE:          error_man.report_custom("SymbolTable", " NONE given to symbol table", true);
        case ValType::STRING:
        {
            // Strings and structs need to have a size given to us
            if(memory == 0) { error_man.report_custom("SymbolTable", "STRING added to symbol table without size", true); }
            mem_request = memory;
            break;
        }
        /*
        case ValType::STRUCT:
        {
            // Strings and structs need to have a size given to us
            if(memory == 0) { error_man.report_custom("SymbolTable", "STRUCT added to symbol table without size", true); }
            mem_request = memory;
            break;
        }
        */
        default:
            error_man.report_custom("SymbolTable", "Default reached in ValType matching", true);
            break;
        }

        // Safety check - If things are build correctly by compiler this should never happen
        if(memory_man.is_id_mapped(symbol))
        {
            std::cerr << "DEVELOPER ERROR : The symbol \" " << symbol << "\" was previously mapped within the memory manager" << std::endl;
            error_man.report_custom("SymbolTable", "Item given to symbol table already exists within the memory map. This is a developer error", true);
        }

        std::cout << "Alloc " << mem_request << " for " << symbol << std::endl;
        // Attempt to 'allocate' memory
        if(!memory_man.alloc_mem(symbol, mem_request))
        {
            error_man.report_out_of_memory(symbol, memory, Memory::MAX_GLOBAL_MEMORY);
        }
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