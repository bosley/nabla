#ifndef DEL_MEMORY_HPP
#define DEL_MEMORY_HPP

#include <map>
#include <string>

#include <libnabla/VSysSettings.hpp>

namespace DEL
{
    class SymbolTable;

    class Memory
    {
    public:
        static constexpr int MAX_GLOBAL_MEMORY = NABLA::VSYS::NABLA_VSYS_SETTINGS_GLOBAL_MEMORY_BYTES;

        Memory();
        ~Memory();

        struct MemAlloc
        {
            uint64_t bytes_alloced;
            uint64_t bytes_requested;
            uint64_t start_pos;
        };

        bool is_id_mapped(std::string id);

        MemAlloc get_mem_info(std::string id);


        friend SymbolTable;
        
    private:

        bool alloc_mem(std::string id, uint64_t minimum_size);

        uint64_t currently_allocated_bytes;
        std::map<std::string, MemAlloc> memory_map;
    };
}

#endif