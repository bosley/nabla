#ifndef DEL_PARTS_GENERATORS_HPP
#define DEL_PARTS_GENERATORS_HPP

#include <string>
#include <vector>
#include <stdint.h>

namespace DEL
{
namespace PARTS
{
     /*
        TODO : 

        This causes a lot of code. We can minimize the amount of code generated by packing multiple bytes into a single LSH call, but
        I want to be as verbose as possible during development. This is considered "low hanging fruit" for later optimization
    */
    static std::vector<std::string> generate_store_64(uint8_t num_bytes, uint64_t le_64, std::string id)
    {
        std::vector<uint8_t> bytes;
        bytes.push_back( (le_64 & 0x00000000000000FF) >> 0  );
        bytes.push_back( (le_64 & 0x000000000000FF00) >> 8  );
        bytes.push_back( (le_64 & 0x0000000000FF0000) >> 16 );
        bytes.push_back( (le_64 & 0x00000000FF000000) >> 24 );
        bytes.push_back( (le_64 & 0x000000FF00000000) >> 32 );
        bytes.push_back( (le_64 & 0x0000FF0000000000) >> 40 );
        bytes.push_back( (le_64 & 0x00FF000000000000) >> 48 );
        bytes.push_back( (le_64 & 0xFF00000000000000) >> 56 );

        // Generate code to shift everything into the represented value on init

        // We could do this using significantly fewer registers (double up the bytes, loop and shift etc)
        // but for development I want to be as verbose as possible so any issues can be more easily found

        std::vector<std::string> instructions;

        instructions.push_back("\n\t; ---- " + id + " ----\n");

        for(int8_t v = 0; v < num_bytes; v++)
        {
            // Create the lsh command. i.e lsh r0 $0 $56
            std::string p1 = "\tlsh r" + std::to_string(v) + " $";
            std::string p2 = std::to_string(bytes[v]) + " $" + std::to_string(((v)*8)) + "\t ; Byte " + std::to_string(v) + "\n";

            instructions.push_back(p1+p2);
        }

        if( num_bytes > 1)
        {
            instructions.push_back("\n\t; ---- OR value together ---- \n");

            for(uint8_t i = 1; i < num_bytes; i++)
            {
                std::string s = std::to_string(i);

                instructions.push_back("\tor r0 r0 r" + s + "\n" );
            }
        }

        return instructions;
    }
}
}

#endif 