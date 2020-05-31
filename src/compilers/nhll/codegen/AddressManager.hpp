#ifndef NHLL_CODEGEN_ADDRESS_MANAGER
#define NHLL_CODEGEN_ADDRESS_MANAGER

#include <stdint.h>

namespace NHLL
{
    class AddressManager
    {
    public:
        AddressManager();

        struct AddressResult
        {
            uint64_t address;
            uint64_t  num_bits_assigned;
        };

        
        // Reserve the first n bytes of global stack for system operations
        AddressResult reserve_system_space(uint64_t n);

        // Casts the integer to an int64_t, checks size, and assignes appropriate space
        AddressResult new_global_integer(int64_t value);
        AddressResult new_global_double();
        AddressResult new_global_string(uint64_t length);


    private:
        uint64_t global_address_index;
    };
}

#endif 