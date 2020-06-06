#include "Codegen.hpp"

#include <iostream>
#include <vector>
#include <limits>
#include <libnabla/util.hpp>
#include <libnabla/endian.hpp>

namespace DEL
{
    namespace
    {
        std::vector<std::string> generate_store_64(uint8_t num_bytes, uint64_t le_64, std::string id)
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

    // ----------------------------------------------------------
    //
    // ----------------------------------------------------------

    Codegen::Codegen(Errors & err, SymbolTable & symbolTable, Memory & memory) : 
                                                                error_man(err), 
                                                                symbol_table(symbolTable), 
                                                                memory_man(memory),
                                                                building_function(false),
                                                                has_included_math_functions(false)
    {

    }

    // ----------------------------------------------------------
    //
    // ----------------------------------------------------------

    Codegen::~Codegen()
    {
        for(auto & l : program_instructions)
        {
            std::cout << l ;
        }
        std::cout << std::endl;
    }

    // ----------------------------------------------------------
    //
    // ----------------------------------------------------------

    void Codegen::begin_function(std::string name, std::vector<FunctionParam> params)
    {
        if(building_function)
        {
            std::cerr << "Internal Error >>> Codegen asked to start function while building function "
                      << "grammar should have prevented this!!!" << std::endl;
            exit(EXIT_FAILURE);
        }

        building_function = true;

        std::cout << "Codegen >>> begin_function : " << name << std::endl;
    }

    // ----------------------------------------------------------
    //
    // ----------------------------------------------------------

    void Codegen::end_function()
    {
        if(!building_function)
        {
            std::cerr << "Internal Error >>> Codegen asked to end function while not building function "
                      << "grammar should have prevented this!!!" << std::endl;
            exit(EXIT_FAILURE);
        }

        building_function = false;

        std::cout << "Codegen >>> end_function()" << std::endl;
    }
    
    // ----------------------------------------------------------
    //
    // ----------------------------------------------------------

    void Codegen::add_build_in_math_functions()
    {
        if(has_included_math_functions)
        {
            return;
        }

        has_included_math_functions = true;

        /*
            Need to load built_ins.asm

            These built-ins will support POW and MOD, but they need to be written to conform to DEL specifications

            consider adding them directly into source code so we don't have to rely on them for compiling 
        
        */
    }

    // ----------------------------------------------------------
    //
    // ----------------------------------------------------------

    void Codegen::assignment(Intermediate::Assignment assignment)
    {
        /*
            assignment.id                    -> The name of the thing we are assigning for comments
            assignemnt.memory_info           -> Where we need to store the thing
            assignment.assignment_classifier -> How to treat the given data (int, char, real)
            assignment.instructions          -> What to do to the data in RPN form
        */

       std::string remove_words_for_calc = "\n\tpopw r9 ls \t ; Calculation RHS\n\tpopw r8 ls \t ; Calculation LHS\n";

       std::string remove_single_word_for_calc = "\n\tpopw r8 ls \t ; Calculation LHS\n";

       std::string calculation_chunk = "r8 r8 r9 \t ; Perform operation, store in r8\n\tpushw ls r8 \t ; Put result into ls\n";

       std::string calculate_unary = "r8 r8 \t ; Perform unary operation, store in r8\n\tpushw ls r8 \t ; Put result into ls\n";

       bool is_double_variant = (assignment.assignment_classifier == Intermediate::AssignmentClassifier::DOUBLE);

       for(auto & ins : assignment.instructions)
       {
           switch(ins.instruction)
           {
               case Intermediate::InstructionSet::ADD:     
                { 
                    program_instructions.push_back("\n\t; <<< ADDITION >>> \n");
                    program_instructions.push_back(remove_words_for_calc + ((is_double_variant) ? "\tadd.d " : "\tadd ") + calculation_chunk);
                    break;                                          
                }
                case Intermediate::InstructionSet::SUB:     
                { 
                    program_instructions.push_back("\n\t; <<< SUBTRACTION >>> \n");
                    program_instructions.push_back(remove_words_for_calc + ((is_double_variant) ? "\tsub.d " : "\tsub ") + calculation_chunk);
                    break;                                          
                }
                case Intermediate::InstructionSet::DIV:     
                { 
                    program_instructions.push_back("\n\t; <<< DIVISION >>> \n");
                    program_instructions.push_back(remove_words_for_calc + ((is_double_variant) ? "\tdiv.d " : "\tdiv ") + calculation_chunk);
                    break;                                          
                }
                case Intermediate::InstructionSet::MUL:     
                { 
                    program_instructions.push_back("\n\t; <<< MULTIPLICATION >>> \n");
                    program_instructions.push_back(remove_words_for_calc + ((is_double_variant) ? "\tmul.d " : "\tmul ") + calculation_chunk);
                    break;                                          
                }
                case Intermediate::InstructionSet::RSH:     
                { 
                    program_instructions.push_back("\n\t; <<< RIGHT SHIFT >>> \n");
                    program_instructions.push_back(remove_words_for_calc + ( "\tlsh " + calculation_chunk));
                    break;                                          
                }
                case Intermediate::InstructionSet::LSH:     
                { 
                    program_instructions.push_back("\n\t; <<< LEFT SHIFT >>> \n");
                    program_instructions.push_back(remove_words_for_calc + ( "\trsh " + calculation_chunk));
                    break;                                          
                }
                case Intermediate::InstructionSet::BW_OR:   
                { 
                    program_instructions.push_back("\n\t; <<< BITWISE OR >>> \n");
                    program_instructions.push_back(remove_words_for_calc + ( "\tor " + calculation_chunk ));
                    break;                                          
                }
                case Intermediate::InstructionSet::BW_NOT:  
                { 
                    program_instructions.push_back("\n\t; <<< BITWISE NOT >>> \n");
                    program_instructions.push_back(remove_single_word_for_calc + ( "\tnot " + calculate_unary));
                    break;                                          
                }
                case Intermediate::InstructionSet::BW_XOR:  
                { 
                    program_instructions.push_back("\n\t; <<< BITWISE XOR >>> \n");
                    program_instructions.push_back(remove_words_for_calc + ( "\txor " + calculation_chunk));
                    break;                                          
                }
                case Intermediate::InstructionSet::BW_AND:  
                { 
                    program_instructions.push_back("\n\t; <<< BITWISE AND >>> \n");
                    program_instructions.push_back(remove_words_for_calc + ( "\tand " + calculation_chunk));
                    break;                                          
                }
                case Intermediate::InstructionSet::LTE:        error_man.report_custom("Codegen", "Developer: LTE not completed"); break;
                case Intermediate::InstructionSet::LT:         error_man.report_custom("Codegen", "Developer: LT not completed"); break;
                case Intermediate::InstructionSet::GTE:        error_man.report_custom("Codegen", "Developer: GTE not completed"); break;
                case Intermediate::InstructionSet::GT:         error_man.report_custom("Codegen", "Developer: GT not completed"); break;
                case Intermediate::InstructionSet::EQ:         error_man.report_custom("Codegen", "Developer: EQ not completed"); break;
                case Intermediate::InstructionSet::NE:         error_man.report_custom("Codegen", "Developer: NE not completed"); break;
                case Intermediate::InstructionSet::OR:         error_man.report_custom("Codegen", "Developer: OR not completed"); break;
                case Intermediate::InstructionSet::AND:        error_man.report_custom("Codegen", "Developer: AND not completed"); break;
                case Intermediate::InstructionSet::NEGATE:     error_man.report_custom("Codegen", "Developer: NEGATE not completed"); break;
                case Intermediate::InstructionSet::LOAD_BYTE:  error_man.report_custom("Codegen", "Developer: LOAD_BYTE not completed"); break;
                case Intermediate::InstructionSet::STORE_BYTE: error_man.report_custom("Codegen", "Developer: STORE_BYTE not completed"); break;
                case Intermediate::InstructionSet::LOAD_WORD:
                {
                    program_instructions.push_back("\n\t; <<< LOAD WORD >>> \n");
                    uint64_t word_address = std::stoull(ins.value);

                    // Generate a register with the address for the destination
                    std::vector<std::string> store_ins = generate_store_64(8, word_address, "Address of thing in expression");
                    program_instructions.insert(program_instructions.end(), store_ins.begin(), store_ins.end());

                    // Load GS address at r0 to r0
                    program_instructions.push_back("\n\tldw r0 r0(gs)\t; Load value of thing for expression\n");
                    program_instructions.push_back("\tpushw ls r0\t; Push value to local stack for calculation\n");
                    break;
                }
                case Intermediate::InstructionSet::STORE_WORD: error_man.report_custom("Codegen", "Developer: STORE_WORD not completed"); break;
                case Intermediate::InstructionSet::CALL:       error_man.report_custom("Codegen", "Developer: CALL not completed"); break;
                case Intermediate::InstructionSet::GET_RESULT:  
                {
                    program_instructions.push_back("\n\t; <<< GET RESULT >>> \n");

                    // Get the memory information for destination
                    uint64_t mem_start = ENDIAN::conditional_to_le_64(assignment.memory_info.start_pos);
                
                    // Generate a register with the address for the destination
                    std::vector<std::string> store_ins = generate_store_64(8, mem_start, ("Address for [" + assignment.id + "]"));
                    program_instructions.insert(program_instructions.end(), store_ins.begin(), store_ins.end());

                    // Get the calculation result off the stack
                    program_instructions.push_back("\n\t; ---- Get result ----\n");
                    program_instructions.push_back("\tpopw r8 ls\n");

                    // Store the result at the memory address
                    program_instructions.push_back("\n\t; ---- Store result ---- \n");
                    program_instructions.push_back("\tstw r0(gs) r8\t ; Store in memory\n");
                    break;
                }
                case Intermediate::InstructionSet::USE_RAW:
                {
                    switch(assignment.assignment_classifier)
                    {
                    case Intermediate::AssignmentClassifier::INTEGER:  setup_integer(assignment.id, ins.value); break;
                    case Intermediate::AssignmentClassifier::DOUBLE:   setup_double(assignment.id, ins.value );  break;
                    case Intermediate::AssignmentClassifier::CHAR:     setup_string(assignment.id, ins.value );  break;
                    default:
                        error_man.report_custom("Codegen", "Developer error: Assignment Classifier switch reached default", true);
                        break;
                    }
                    break;
                }
                case Intermediate::InstructionSet::POW:
                {
                    if(!has_included_math_functions){ add_build_in_math_functions(); }

                    // TODO 
                    
                    // Need to load things into registers used for power function
                    // Call that function
                    // Get the resul and store 

                    error_man.report_custom("Codegen", "Developer: POW not completed"); break;
                    break;
                }
                case Intermediate::InstructionSet::MOD:
                {
                    if(!has_included_math_functions){ add_build_in_math_functions(); }

                    // TODO 

                    // Need to load things into registers used for power function
                    // Call that function
                    // Get the resul and store 

                    error_man.report_custom("Codegen", "Developer: MOD not completed"); break;
                    break;
                }
                default:
                    error_man.report_custom("Codegen", "Developer error : Default accessed in assignment", true);
                    break;
           }
       }
    }

    // ----------------------------------------------------------
    //
    // ----------------------------------------------------------

    void Codegen::setup_integer(std::string id, std::string value)
    {
        program_instructions.push_back("\n\t; <<< SETUP INT >>> \n");

        // Get the numerical value
        int64_t i_value = std::stoll(value);

        // Conditionally little-endian the thing and convert it to an unsigned value
        uint64_t unsigned_value = ENDIAN::conditional_to_le_64(static_cast<uint64_t>(i_value));

        uint8_t bytes = 8;
        if(i_value < std::numeric_limits<int8_t>::max() && i_value > std::numeric_limits<int8_t>::min()){          bytes = 1; }
        else if (i_value < std::numeric_limits<int16_t>::max() && i_value > std::numeric_limits<int16_t>::min()){  bytes = 2; }
        else if(i_value < std::numeric_limits<int32_t>::max() && i_value > std::numeric_limits<int32_t>::min()) {  bytes = 4; }

        // Generate the store for 64 bit
        std::vector<std::string> store_ins = generate_store_64(bytes, unsigned_value, id);

        // Merge store instructions into program init
        program_instructions.insert(program_instructions.end(), store_ins.begin(), store_ins.end());

        program_instructions.push_back("\n\t; ---- Move the value ----\n");
        program_instructions.push_back("\tpushw ls r0\t ; Place on local stack for calculation\n");
    }

    // ----------------------------------------------------------
    //
    // ----------------------------------------------------------

    void Codegen::setup_double(std::string id, std::string value)
    {
        program_instructions.push_back("\n\t; <<< SETUP REAL >>> \n");

        double d = std::stod(value);

        std::cout << "DOUBLE VAL : " << d << std::endl;

        uint64_t unsigned_value = ENDIAN::conditional_to_le_64(
                                    UTIL::convert_double_to_uint64(std::stod(value)
                                    )
                                );
        
        // Generate the store the real
        std::vector<std::string> store_ins = generate_store_64(8, unsigned_value, id);

        // Merge store instructions into program init
        program_instructions.insert(program_instructions.end(), store_ins.begin(), store_ins.end());

        program_instructions.push_back("\n\t; ---- Move the value ----\n");
        program_instructions.push_back("\tpushw ls r0\t ; Place on local stack for calculation\n");
    }

    // ----------------------------------------------------------
    //
    // ----------------------------------------------------------

    void Codegen::setup_string(std::string id, std::string value)
    {
        program_instructions.push_back("\n\t; <<< SETUP STR >>> \n");
    }

}