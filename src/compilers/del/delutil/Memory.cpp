#include "Memory.hpp"

#include <iostream>

namespace DEL
{
    // ----------------------------------------------------------
    //
    // ----------------------------------------------------------

    Memory::Memory() : currently_allocated_bytes(START_ADDRESS_SPACE)
    {

    }

    // ----------------------------------------------------------
    //
    // ----------------------------------------------------------

    Memory::~Memory()
    {

    }

    // ----------------------------------------------------------
    //
    // ----------------------------------------------------------

    bool Memory::is_id_mapped(std::string id)
    {
        if(memory_map.find(id) != memory_map.end())
        {
            return true;
        }
        return false;
    }

    // ----------------------------------------------------------
    //
    // ----------------------------------------------------------

    Memory::MemAlloc Memory::get_mem_info(std::string id)
    {
        if(memory_map.find(id) != memory_map.end())
        {
            return memory_map[id];
        }

        //std::cout << "MEM : " << id << " not found " << std::endl;

        return MemAlloc{ 0, 0, START_ADDRESS_SPACE};
    }

    // ----------------------------------------------------------
    //
    // ----------------------------------------------------------

    uint64_t Memory::get_currently_allocated_bytes_amnt() const
    {
        return currently_allocated_bytes;
    }

    // ----------------------------------------------------------
    //
    // ----------------------------------------------------------

    bool Memory::alloc_mem(std::string id, uint64_t minimum_size)
    {
        if((currently_allocated_bytes + minimum_size) > MAX_GLOBAL_MEMORY )
        {
            return false;
        }

        MemAlloc allocated; 

        allocated.bytes_requested = minimum_size;

        uint8_t additional = (minimum_size % 8);

        if( additional > 0)
        {
            minimum_size = (8 - additional);
        }

        //std::cout << "Bytes alloced for " << id << " " << minimum_size << std::endl;

        allocated.bytes_alloced = minimum_size;

        allocated.start_pos = currently_allocated_bytes;;

        currently_allocated_bytes += (allocated.bytes_alloced + 1);

        memory_map[id] = allocated;

        return true;
    }


}