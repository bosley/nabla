#include "bytegen.hpp"

#include "insmanifest.hpp"

namespace SOLACE
{
    namespace
    {
        // This might seem silly, but it allows us to change register addresses without picking out 
        // ALL the aread that they are being set
        uint8_t integerToRegister(int16_t reg)
        {
            switch(reg)
            {
                case 0 : return MANIFEST::REGISTER_0 ;
                case 1 : return MANIFEST::REGISTER_1 ;
                case 2 : return MANIFEST::REGISTER_2 ;
                case 3 : return MANIFEST::REGISTER_3 ;
                case 4 : return MANIFEST::REGISTER_4 ;
                case 5 : return MANIFEST::REGISTER_5 ;
                case 6 : return MANIFEST::REGISTER_6 ;
                case 7 : return MANIFEST::REGISTER_7 ;
                case 8 : return MANIFEST::REGISTER_8 ;
                case 9 : return MANIFEST::REGISTER_9 ;
                case 10: return MANIFEST::REGISTER_10;
                case 11: return MANIFEST::REGISTER_11;
                case 12: return MANIFEST::REGISTER_12;
                case 13: return MANIFEST::REGISTER_13;
                case 14: return MANIFEST::REGISTER_14;
                case 15: return MANIFEST::REGISTER_15;
                default: 
                    std::cerr << "Someone tried something silly" << std::endl;
                    exit(EXIT_FAILURE); 
                    break;
            }
        }

        // Helper for debugging
        void dumpInstruction(Bytegen::Instruction ins)
        {
            std::cout << std::endl << "\t"
            << std::bitset<8>(ins.bytes[0]) << " | " 
            << std::bitset<8>(ins.bytes[1]) << " | " 
            << std::bitset<8>(ins.bytes[2]) << " | " 
            << std::bitset<8>(ins.bytes[3]) << " | " 
            << std::bitset<8>(ins.bytes[4]) << " | " 
            << std::bitset<8>(ins.bytes[5]) << " | " 
            << std::bitset<8>(ins.bytes[6]) << " | " 
            << std::bitset<8>(ins.bytes[7]) << "   " 
            << std::endl << std::endl;
        }
    }

    // ------------------------------------------------------------------------
    // Bytegen
    // ------------------------------------------------------------------------
    
    Bytegen::Bytegen() : functionCounter(0)
    {
        //MANIFEST::displayManifest();

    }
    
    // ------------------------------------------------------------------------
    // createFunctionStart
    // ------------------------------------------------------------------------
    
    Bytegen::Instruction Bytegen::createFunctionStart(std::string name, uint64_t numInstructions, uint32_t &address)
    {
        Instruction ins;

        address = functionCounter;

        functionCounter++;

        return ins;
    }

    // ------------------------------------------------------------------------
    // createFunctionEnd
    // ------------------------------------------------------------------------

    Bytegen::Instruction Bytegen::createFunctionEnd()
    {
        std::cout << "Bytegen::createFunctionEnd()" << std::endl;

        return Instruction{ MANIFEST::INS_FUNCTION_END, 0, 0, 0, 0, 0, 0, 0 };
    }

    // ------------------------------------------------------------------------
    // createConstantString
    // ------------------------------------------------------------------------
    
    std::vector<uint8_t> Bytegen::createConstantString(std::string val)
    {
        std::vector<uint8_t> result;

        std::cout << "Bytegen::createConstantString(" << val << ")" << std::endl;
        
        result.push_back( MANIFEST::CONST_STR      );
        for(auto& v : val)
        {
            result.push_back(static_cast<uint8_t>(v));
        }

        return result;
    }

    // ------------------------------------------------------------------------
    // createConstantInt
    // ------------------------------------------------------------------------
    
    std::vector<uint8_t> Bytegen::createConstantInt   (uint32_t val)
    {
        std::vector<uint8_t> result;

        std::cout << "Bytegen::createConstantInt(" << val << ")" << std::endl;

        result.push_back( MANIFEST::CONST_INT      );
        result.push_back( (val & 0xFF000000) >> 24 );
        result.push_back( (val & 0x00FF0000) >> 16 );
        result.push_back( (val & 0x0000FF00) >> 8  );
        result.push_back( (val & 0x000000FF) >> 0  );

        return result;
    }

    // ------------------------------------------------------------------------
    // createConstantDouble
    // ------------------------------------------------------------------------
    
    std::vector<uint8_t> Bytegen::createConstantDouble(double dval)
    {
        std::vector<uint8_t> result;

        std::cout << "Bytegen::createConstantDouble(" << dval << ")" << std::endl;

        uint32_t val = static_cast<uint32_t>(dval + 0.5);

        result.push_back( MANIFEST::CONST_DBL      );
        result.push_back( (val & 0xFF000000) >> 24 );
        result.push_back( (val & 0x00FF0000) >> 16 );
        result.push_back( (val & 0x0000FF00) >> 8  );
        result.push_back( (val & 0x000000FF) >> 0  );

        return result;
    }

    // ------------------------------------------------------------------------
    // createConstantDouble
    // ------------------------------------------------------------------------
    
    Bytegen::Instruction Bytegen::createArithmaticInstruction(ArithmaticTypes type, ArithmaticSetup setup, int16_t arg1, int16_t arg2, int16_t arg3)
    {        
        Instruction ins;

        uint8_t op = 0;

        switch(type)
        {
            case Bytegen::ArithmaticTypes::ADD : std::cout << "Bytegen::createArithmatic::ADD  :: "; op = MANIFEST::INS_ADD ; break;
            case Bytegen::ArithmaticTypes::ADDD: std::cout << "Bytegen::createArithmatic::ADDD :: "; op = MANIFEST::INS_ADDD; break; 
            case Bytegen::ArithmaticTypes::MUL : std::cout << "Bytegen::createArithmatic::MUL  :: "; op = MANIFEST::INS_MUL ; break;
            case Bytegen::ArithmaticTypes::MULD: std::cout << "Bytegen::createArithmatic::MULD :: "; op = MANIFEST::INS_MULD; break; 
            case Bytegen::ArithmaticTypes::DIV : std::cout << "Bytegen::createArithmatic::DIV  :: "; op = MANIFEST::INS_DIV ; break;
            case Bytegen::ArithmaticTypes::DIVD: std::cout << "Bytegen::createArithmatic::DIVD :: "; op = MANIFEST::INS_DIVD; break; 
            case Bytegen::ArithmaticTypes::SUB : std::cout << "Bytegen::createArithmatic::SUB  :: "; op = MANIFEST::INS_SUB ; break;
            case Bytegen::ArithmaticTypes::SUBD: std::cout << "Bytegen::createArithmatic::SUBD :: "; op = MANIFEST::INS_SUBD; break; 
            default:      exit(EXIT_FAILURE);  return ins; // Keep that compiler happy.
        }

        // Set id
        switch(setup)
        {
            case Bytegen::ArithmaticSetup::REG_REG: 
            {
                std::cout << " :: REG_REG "; 
                ins.bytes[0] = (op);
                ins.bytes[1] = (integerToRegister(arg1));
                ins.bytes[2] = (integerToRegister(arg2));
                ins.bytes[3] = (integerToRegister(arg3));
                ins.bytes[4] = 0xFF;
                ins.bytes[5] = 0xFF;
                ins.bytes[6] = 0xFF;
                ins.bytes[7] = 0xFF;
                break; 
            }

            case Bytegen::ArithmaticSetup::NUM_REG: 
            {
                std::cout << " :: NUM_REG "; 
                op = op | 0x02; 
                ins.bytes[0] = (op);
                ins.bytes[1] = (integerToRegister(arg1));

                uint16_t num = static_cast<uint16_t>(arg2);

                ins.bytes[2] = ( (num & 0xFF00) >> 8 ) ;

                ins.bytes[3] = ( (num & 0x00FF) >> 0 ) ;

                ins.bytes[4] = (integerToRegister(arg3));

                ins.bytes[5] = 0xFF;
                ins.bytes[6] = 0xFF;
                ins.bytes[7] = 0xFF;
                break; 
            }

            case Bytegen::ArithmaticSetup::REG_NUM: 
            {
                std::cout << " :: REG_NUM "; 
                op = op | 0x01; 
                ins.bytes[0] = (op);
                ins.bytes[1] = (integerToRegister(arg1));
                ins.bytes[2] = (integerToRegister(arg2));

                uint16_t num = static_cast<uint16_t>(arg3);

                ins.bytes[3] = ( (num & 0xFF00) >> 8 ) ;

                ins.bytes[4] = ( (num & 0x00FF) >> 0 ) ;

                ins.bytes[5] = 0xFF;
                ins.bytes[6] = 0xFF;
                ins.bytes[7] = 0xFF;
                break; 
            }

            case Bytegen::ArithmaticSetup::NUM_NUM: 
            {
                std::cout << " :: NUM_NUM "; 
                op = op | 0x03; 
                ins.bytes[0] = (op);
                ins.bytes[1] = (integerToRegister(arg1));

                uint16_t num = static_cast<uint16_t>(arg2);

                ins.bytes[2] = ( (num & 0xFF00) >> 8 ) ;
                ins.bytes[3] = ( (num & 0x00FF) >> 0 ) ;

                uint16_t num1 = static_cast<uint16_t>(arg3);

                ins.bytes[4] = ( (num1 & 0xFF00) >> 8 ) ;
                ins.bytes[5] = ( (num1 & 0x00FF) >> 0 ) ;

                ins.bytes[6] = 0xFF;
                ins.bytes[7] = 0xFF;
                break; 
            }

            default:      
                std::cerr << "SOMETHING AWFUL HAPPENED DON'T TRUST ANYTHING" << std::endl;
                return ins; // Keep that compiler happy.
        }

#warning Remove this output after debugging

        std::cout << "Arg1: " << arg1 << " Arg2: " << arg2 << " Arg3: " << arg3 << std::endl;
        dumpInstruction(ins);

        return ins;
    }


    // ------------------------------------------------------------------------
    // createBranchInstruction
    // ------------------------------------------------------------------------
    
    Bytegen::Instruction Bytegen::createBranchInstruction(BranchTypes type, uint8_t reg1, uint8_t reg2, uint32_t location)
    {
        Instruction ins;

        // Set the instruction op
        switch(type)
        {
            case Bytegen::BranchTypes::BGT  : ins.bytes[0] = MANIFEST::INS_BGT  ; break; 
            case Bytegen::BranchTypes::BGTE : ins.bytes[0] = MANIFEST::INS_BGTE ; break; 
            case Bytegen::BranchTypes::BLT  : ins.bytes[0] = MANIFEST::INS_BLT  ; break; 
            case Bytegen::BranchTypes::BLTE : ins.bytes[0] = MANIFEST::INS_BLTE ; break; 
            case Bytegen::BranchTypes::BEQ  : ins.bytes[0] = MANIFEST::INS_BEQ  ; break; 
            case Bytegen::BranchTypes::BNE  : ins.bytes[0] = MANIFEST::INS_BNE  ; break; 
            case Bytegen::BranchTypes::BGTD : ins.bytes[0] = MANIFEST::INS_BGTD ; break;
            case Bytegen::BranchTypes::BGTED: ins.bytes[0] = MANIFEST::INS_BGTED; break;
            case Bytegen::BranchTypes::BLTD : ins.bytes[0] = MANIFEST::INS_BLTD ; break;
            case Bytegen::BranchTypes::BLTED: ins.bytes[0] = MANIFEST::INS_BLTED; break;
            case Bytegen::BranchTypes::BEQD : ins.bytes[0] = MANIFEST::INS_BEQD ; break;
            case Bytegen::BranchTypes::BNED : ins.bytes[0] = MANIFEST::INS_BNED ; break;
            default:                          return ins; // Keep that compiler happy.
        }

        ins.bytes[1] = integerToRegister(reg1);
        ins.bytes[2] = integerToRegister(reg2);
        ins.bytes[3] = (location & 0xFF000000) >> 24 ;
        ins.bytes[4] = (location & 0x00FF0000) >> 16 ;
        ins.bytes[5] = (location & 0x0000FF00) >> 8  ;
        ins.bytes[6] = (location & 0x000000FF) >> 0  ;
        ins.bytes[7] = 0xFF;

        dumpInstruction(ins);

        return ins;
    }

} // End of namespace