#include "AddressManager.hpp"
#include <limits>

#include <iostream>

namespace NHLL
{
    AddressManager::AddressManager() : global_address_index(0)
    {

    }

    AddressManager::AddressResult AddressManager::reserve_system_space(uint64_t n)
    {
        uint64_t  assigned = n * 8;
        uint64_t addr = global_address_index;

        global_address_index += n;
        
        return AddressResult { addr, assigned };
    }

    AddressManager::AddressResult AddressManager::new_global_integer(int64_t val)
    {
        // For now we are setting all global integers to int64s
        uint64_t  assigned = 0;
        uint64_t addr = global_address_index;

        assigned = 64;
        global_address_index += 8;

        return AddressResult { addr, assigned };
    }

    AddressManager::AddressResult AddressManager::new_global_double()
    {
        uint64_t addr = global_address_index;
        global_address_index += 8;
        return AddressResult{ addr, 64 };
    }

    AddressManager::AddressResult AddressManager::new_global_string(uint64_t length)
    {
        uint64_t addr = global_address_index;
        global_address_index += length;
        return AddressResult{ addr, length * 8};
    }
}