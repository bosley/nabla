/*
    This file is getting large, so I think its a good idea to write a comment up top to describe somethings that still need to be done and what is done. 

    What is done now :  int / char / double expressions are all complete
        Those two things need to have some functions generated to support them. 
        Other than that : - + * / || && ! xor and not or lsh rsh LT LTE GT GTE EQ NE have been fleshed out
        They have been mostly hand tested. Once the remainind 'assignment' operations have been completed, we will need to setup a test mechanism to ensure
        that all of these compile into things that actually work. 

        Getting values from function calls has not yet been completed. That is below and will trigger the error manager to laugh at us if we attempt
        to use a function call anywhere in an expression. At this point I'm pretty sure the grammer doesn't even let us do that, but once it does we will 
        DEFINITELY be laughed at by the code generator. 


    Future:

        So this is my first time doing this. I HATE the mashing of asm into strings and vectors. I want to bang out this POC and then create some other classes that 
        are used to represent individual actions and have an understanding of the ASM that we can call into from here to generate the code. That way if the ASM changes later
        (god I hope not) we can then just change the syntax setup in a singular place and BOOM everything will be updated
*/

#include "Codegen.hpp"
#include "Generators.hpp"

#include <iostream>
#include <vector>
#include <limits>
#include <libnabla/util.hpp>
#include <libnabla/endian.hpp>

namespace DEL
{
    namespace
    {
        //  Setup a conditional check a < 3; a || b etc
        //
        std::vector<std::string> setup_check(uint64_t label_id, std::string comparison, std::string removal)
        {
            std::vector<std::string> instructions;
            std::string label = "conditional_check_" + std::to_string(label_id);
            std::string complete = "conditional_complete_" + std::to_string(label_id);
            comparison = comparison + label + "\n\n";
            instructions.push_back(removal + "\n");
            instructions.push_back(comparison);
            instructions.push_back("\tmov r8 $0\t; False\n\n");
            instructions.push_back("\tjmp " + complete + "\n");
            instructions.push_back("\n" + label + ":\n");
            instructions.push_back("\tmov r8 $1\t; True\n");
            instructions.push_back("\n" + complete + ":\n");
            instructions.push_back("\n\tpushw ls r8 \t ; Put result into ls\n");
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
                                                                label_id(0)
    {

    }

    // ----------------------------------------------------------
    //
    // ----------------------------------------------------------

    Codegen::~Codegen()
    {
        
    }

    // ----------------------------------------------------------
    //
    // ----------------------------------------------------------

    std::vector<std::string> Codegen::indicate_complete()
    {
        std::cout << "Codegen::indicate_complete()" << std::endl;

        // Lock the symbol table so if an error comes out the dev (me) knows they did something dumb
        symbol_table.lock();

        std::vector<std::string> result; 
        
        asm_support.import_init_start(result);

        uint64_t memory_alloc = memory_man.get_currently_allocated_bytes_amnt();

        // Memory manager has reserved some room at the front of GS for information about the program
        // Right now its set to 64 bytes

        // 8 bytes
        result.push_back(".int64 __STACK_FRAME_OFFSET__ " + std::to_string(Memory::START_ADDRESS_SPACE) + "\n");
        result.push_back(".int64 __MEM_ALLOC_COUNTER__ " + std::to_string(memory_alloc) + "\n");

        // These are reserved by the memory manager and need to be populated for correctness, used or not
        result.push_back(".int64 __PLACE_HOLDER__0__ 0\n");
        result.push_back(".int64 __PLACE_HOLDER__1__ 0\n");
        result.push_back(".int64 __PLACE_HOLDER__2__ 0\n");
        result.push_back(".int64 __PLACE_HOLDER__3__ 0\n");
        result.push_back(".int64 __PLACE_HOLDER__4__ 0\n");
        result.push_back(".int64 __PLACE_HOLDER__5__ 0\n");

        // Add the init function - All globals in reserved space must be pushed before this
        asm_support.import_init_func(result);

        // Add the functions that were triggered to be added by something that the user was doing (math or something lame)
        result.insert(result.end(), program_init.begin(), program_init.end());

        program_init.clear();

        // Add all of the user function's and instructions
        result.insert(result.end(), program_instructions.begin(), program_instructions.end());

        program_instructions.clear();

        return result;
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

        current_function = new PARTS::FunctionRepresentation(name, params);
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
        label_id = 0;

        std::cout << "Codegen >>> end_function()" << std::endl;

        std::vector<std::string> final_function_instructions = current_function->building_complete();

        program_instructions.insert(program_instructions.end(), final_function_instructions.begin(), final_function_instructions.end());

        delete current_function;
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

       /*
            Raw numbers : we build them with shifts and ors, then place them in the local function stack for calculation
            Operations  : Pop the ls to acquire the LHS and RHS of the calculation, perform the calculation, then store the result in ls by pushing
            Saving      : If we are saving the value, we build the given address using shifts and ors, pop the value off ls, then store the word at its address
            Loading     : We build the address, get the item into a register using ldw, then push it onto the local stack so the next operation can use it
       */

       // Add the bytes for the function's stack frame
       current_function->bytes_required += assignment.memory_info.bytes_alloced;

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
                    current_function->instructions.push_back("\n\t; <<< ADDITION >>> \n");
                    current_function->instructions.push_back(remove_words_for_calc + ((is_double_variant) ? "\tadd.d " : "\tadd ") + calculation_chunk);
                    break;                                          
                }
                case Intermediate::InstructionSet::SUB:     
                { 
                    current_function->instructions.push_back("\n\t; <<< SUBTRACTION >>> \n");
                    current_function->instructions.push_back(remove_words_for_calc + ((is_double_variant) ? "\tsub.d " : "\tsub ") + calculation_chunk);
                    break;                                          
                }
                case Intermediate::InstructionSet::DIV:     
                { 
                    current_function->instructions.push_back("\n\t; <<< DIVISION >>> \n");
                    current_function->instructions.push_back(remove_words_for_calc + ((is_double_variant) ? "\tdiv.d " : "\tdiv ") + calculation_chunk);
                    break;                                          
                }
                case Intermediate::InstructionSet::MUL:     
                { 
                    current_function->instructions.push_back("\n\t; <<< MULTIPLICATION >>> \n");
                    current_function->instructions.push_back(remove_words_for_calc + ((is_double_variant) ? "\tmul.d " : "\tmul ") + calculation_chunk);
                    break;                                          
                }
                case Intermediate::InstructionSet::RSH:     
                { 
                    current_function->instructions.push_back("\n\t; <<< RIGHT SHIFT >>> \n");
                    current_function->instructions.push_back(remove_words_for_calc + ( "\tlsh " + calculation_chunk));
                    break;                                          
                }
                case Intermediate::InstructionSet::LSH:     
                { 
                    current_function->instructions.push_back("\n\t; <<< LEFT SHIFT >>> \n");
                    current_function->instructions.push_back(remove_words_for_calc + ( "\trsh " + calculation_chunk));
                    break;                                          
                }
                case Intermediate::InstructionSet::BW_OR:   
                { 
                    current_function->instructions.push_back("\n\t; <<< BITWISE OR >>> \n");
                    current_function->instructions.push_back(remove_words_for_calc + ( "\tor " + calculation_chunk ));
                    break;                                          
                }
                case Intermediate::InstructionSet::BW_NOT:  
                { 
                    current_function->instructions.push_back("\n\t; <<< BITWISE NOT >>> \n");
                    current_function->instructions.push_back(remove_single_word_for_calc + ( "\tnot " + calculate_unary));
                    break;                                          
                }
                case Intermediate::InstructionSet::BW_XOR:  
                { 
                    current_function->instructions.push_back("\n\t; <<< BITWISE XOR >>> \n");
                    current_function->instructions.push_back(remove_words_for_calc + ( "\txor " + calculation_chunk));
                    break;                                          
                }
                case Intermediate::InstructionSet::BW_AND:  
                { 
                    current_function->instructions.push_back("\n\t; <<< BITWISE AND >>> \n");
                    current_function->instructions.push_back(remove_words_for_calc + ( "\tand " + calculation_chunk));
                    break;                                          
                }
                case Intermediate::InstructionSet::LTE:        
                {
                    current_function->instructions.push_back("\n\t; <<< LTE >>> \n");
                    std::string comparison = (assignment.assignment_classifier == Intermediate::AssignmentClassifier::DOUBLE) ? "\tblte.d r8 r9 " : "\tblte r8 r9 ";
                    std::vector<std::string> c = setup_check(label_id, comparison, remove_words_for_calc);
                    current_function->instructions.insert(current_function->instructions.end(), c.begin(), c.end());
                    label_id++;
                    break;
                }
                case Intermediate::InstructionSet::LT:  
                {
                    current_function->instructions.push_back("\n\t; <<< LT >>> \n");
                    std::string comparison = (assignment.assignment_classifier == Intermediate::AssignmentClassifier::DOUBLE) ? "\tblt.d r8 r9 " : "\tblt r8 r9 ";
                    std::vector<std::string> c = setup_check(label_id, comparison, remove_words_for_calc);
                    current_function->instructions.insert(current_function->instructions.end(), c.begin(), c.end());
                    label_id++;
                    break;
                }
                case Intermediate::InstructionSet::GTE: 
                {
                    current_function->instructions.push_back("\n\t; <<< GTE >>> \n");
                    std::string comparison = (assignment.assignment_classifier == Intermediate::AssignmentClassifier::DOUBLE) ? "\tbgte.d r8 r9 " : "\tbgte r8 r9 ";
                    std::vector<std::string> c = setup_check(label_id, comparison, remove_words_for_calc);
                    current_function->instructions.insert(current_function->instructions.end(), c.begin(), c.end());
                    label_id++;
                    break;
                }
                case Intermediate::InstructionSet::GT:  
                {
                    current_function->instructions.push_back("\n\t; <<< GT >>> \n");
                    std::string comparison = (assignment.assignment_classifier == Intermediate::AssignmentClassifier::DOUBLE) ? "\tbgt.d r8 r9 " : "\tbgt r8 r9 ";
                    std::vector<std::string> c = setup_check(label_id, comparison, remove_words_for_calc);
                    current_function->instructions.insert(current_function->instructions.end(), c.begin(), c.end());
                    label_id++;
                    break;
                }
                case Intermediate::InstructionSet::EQ:  
                {
                    current_function->instructions.push_back("\n\t; <<< EQ >>> \n");
                    std::string comparison = (assignment.assignment_classifier == Intermediate::AssignmentClassifier::DOUBLE) ? "\tbeq.d r8 r9 " : "\tbeq r8 r9 ";
                    std::vector<std::string> c = setup_check(label_id, comparison, remove_words_for_calc);
                    current_function->instructions.insert(current_function->instructions.end(), c.begin(), c.end());
                    label_id++;
                    break;
                }
                case Intermediate::InstructionSet::NE:  
                {
                    current_function->instructions.push_back("\n\t; <<< NE >>> \n");
                    std::string comparison = (assignment.assignment_classifier == Intermediate::AssignmentClassifier::DOUBLE) ? "\tbne.d r8 r9 " : "\tbne r8 r9 ";
                    std::vector<std::string> c = setup_check(label_id, comparison, remove_words_for_calc);
                    current_function->instructions.insert(current_function->instructions.end(), c.begin(), c.end());
                    label_id++;
                    break;
                }
                case Intermediate::InstructionSet::OR:
                {
                    current_function->instructions.push_back("\n\t; <<< OR >>> \n");
                    current_function->instructions.push_back(remove_words_for_calc);
                    current_function->instructions.push_back("\n\tmov r7 $0\t; Comparison value");
                    std::string true_label = "OR_is_true_"    + std::to_string(label_id);
                    std::string complete   = "OR_is_complete_" + std::to_string(label_id);
                    std::string comparison = (assignment.assignment_classifier == Intermediate::AssignmentClassifier::DOUBLE) ? "bgt.d " : "bgt ";
                    current_function->instructions.push_back("\n\n\t" + comparison + "r8 r7 " + true_label);
                    current_function->instructions.push_back("\n\t" + comparison + "r9 r7 " + true_label);
                    current_function->instructions.push_back("\n\n\tmov r8 $0 ; False");
                    current_function->instructions.push_back("\n\tjmp " + complete + "\n\n");
                    current_function->instructions.push_back(true_label + ":\n");
                    current_function->instructions.push_back("\n\tmov r8 $1 ; False\n\n");
                    current_function->instructions.push_back(complete + ":\n");
                    current_function->instructions.push_back("\n\tpushw ls r8 \t ; Put result into ls\n");
                    label_id++;
                    break;
                }
                case Intermediate::InstructionSet::AND:
                {
                    current_function->instructions.push_back("\n\t; <<< AND >>> \n");
                    current_function->instructions.push_back(remove_words_for_calc);
                    current_function->instructions.push_back("\n\tmov r7 $0\t; Comparison value\n\n");
                    std::string first_true  = "AND_first_true_" + std::to_string(label_id);
                    std::string second_true = "AND_second_true_" + std::to_string(label_id);
                    std::string complete    = "AND_complete_" + std::to_string(label_id);
                    std::string comparison = (assignment.assignment_classifier == Intermediate::AssignmentClassifier::DOUBLE) ? "bgt.d " : "bgt ";
                    current_function->instructions.push_back("\t" + comparison + "r8 r7 " + first_true + "\n\n");
                    current_function->instructions.push_back("\tmov r8 $0\t; False\n\n");
                    current_function->instructions.push_back("\tjmp " + complete + "\n\n");
                    current_function->instructions.push_back(first_true + ":\n\n");
                    current_function->instructions.push_back("\t" + comparison + "r9 r7 " + second_true + "\n\n");
                    current_function->instructions.push_back("\tmov r8 $0\t; False\n\n");
                    current_function->instructions.push_back("\tjmp " + complete + "\n\n");
                    current_function->instructions.push_back(second_true + ":\n\n");
                    current_function->instructions.push_back("\tmov r8 $1\n\n");
                    current_function->instructions.push_back(complete + ":\n\n");
                    current_function->instructions.push_back("\n\tpushw ls r8 \t ; Put result into ls\n");
                    label_id++;
                    break;
                }
                case Intermediate::InstructionSet::NEGATE:     
                {
                    current_function->instructions.push_back("\n\t; <<< NEGATE >>> \n");
                    current_function->instructions.push_back(remove_single_word_for_calc);
                    current_function->instructions.push_back("\n\tmov r7 $0\t; Comparison\n\n");
                    std::string comparison = (assignment.assignment_classifier == Intermediate::AssignmentClassifier::DOUBLE) ? "bgt.d " : "bgt ";
                    std::string set_zero = "NEGATE_set_zero_" + std::to_string(label_id);
                    std::string set_comp = "NEGATE_complete_" + std::to_string(label_id);
                    current_function->instructions.push_back("\t" + comparison + " r8 r7 " + set_zero + "\n\n");
                    current_function->instructions.push_back("\n\tmov r8 $1\n\tjmp " + set_comp + "\n\n");
                    current_function->instructions.push_back(set_zero + ":\n\t mov r8 $0\n\n" + set_comp + ":\n\n");
                    current_function->instructions.push_back("\n\tpushw ls r8 \t ; Put result into ls\n");
                    label_id++;
                    break;
                }
                case Intermediate::InstructionSet::LOAD_BYTE:
                {
                    current_function->instructions.push_back("\n\t; <<< LOAD BYTE >>> \n");
                    uint64_t word_address = std::stoull(ins.value);

                    // Generate a register with the address for the destination
                    std::vector<std::string> store_ins = PARTS::generate_store_64(8, word_address, "Address of thing in expression");
                    current_function->instructions.insert(current_function->instructions.end(), store_ins.begin(), store_ins.end());

                    // Add the relative address of the item to the start position of the function in memory to acquire the actual destination
                    current_function->instructions.push_back("\n\tldw r1 $0(ls)\t; Start of stack frame\n");
                    current_function->instructions.push_back("\n\tadd r0 r0 r1\t; Item location in function memory\n");

                    // Load GS address at r0 to r0
                    current_function->instructions.push_back("\n\tldb r0 r0(gs)\t; Load value of thing for expression\n");
                    current_function->instructions.push_back("\tpush ls r0\t; Push value to local stack for calculation\n");
                    break;
                }
                case Intermediate::InstructionSet::LOAD_WORD:
                {
                    current_function->instructions.push_back("\n\t; <<< LOAD WORD >>> \n");
                    uint64_t word_address = std::stoull(ins.value);

                    // Generate a register with the address for the destination
                    std::vector<std::string> store_ins = PARTS::generate_store_64(8, word_address, "Address of thing in expression");
                    current_function->instructions.insert(current_function->instructions.end(), store_ins.begin(), store_ins.end());

                    // Add the relative address of the item to the start position of the function in memory to acquire the actual destination
                    current_function->instructions.push_back("\n\tldw r1 $0(ls)\t; Start of stack frame\n");
                    current_function->instructions.push_back("\n\tadd r0 r0 r1\t; Item location in function memory\n");

                    // Load GS address at r0 to r0
                    current_function->instructions.push_back("\n\tldw r0 r0(gs)\t; Load value of thing for expression\n");
                    current_function->instructions.push_back("\tpushw ls r0\t; Push value to local stack for calculation\n");
                    break;
                }
                case Intermediate::InstructionSet::STORE_BYTE:
                {
                    current_function->instructions.push_back("\n\t; <<< STORE BYTE >>> \n");

                    // Get the memory information for destination
                    uint64_t mem_start = ENDIAN::conditional_to_le_64(assignment.memory_info.start_pos);
                
                    // Generate a register with the address for the destination
                    std::vector<std::string> store_ins = PARTS::generate_store_64(8, mem_start, ("Address for [" + assignment.id + "]"));
                    current_function->instructions.insert(current_function->instructions.end(), store_ins.begin(), store_ins.end());

                    // Add the relative address of the item to the start position of the function in memory to acquire the actual destination
                    current_function->instructions.push_back("\n\tldw r1 $0(ls)\t; Start of stack frame\n");
                    current_function->instructions.push_back("\n\tadd r0 r0 r1\t; Item location in function memory\n");

                    // Get the calculation result off the stack
                    current_function->instructions.push_back("\n\t; ---- Get result ----\n");
                    current_function->instructions.push_back("\tpop r8 ls\n");

                    // Store the result at the memory address
                    current_function->instructions.push_back("\n\t; ---- Store result ---- \n");
                    current_function->instructions.push_back("\tstb r0(gs) r8\t ; Store in memory\n");
                    break;
                }
                case Intermediate::InstructionSet::STORE_WORD:  
                {
                    current_function->instructions.push_back("\n\t; <<< STORE WORD >>> \n");

                    // Get the memory information for destination
                    uint64_t mem_start = ENDIAN::conditional_to_le_64(assignment.memory_info.start_pos);
                
                    // Generate a register with the address for the destination
                    std::vector<std::string> store_ins = PARTS::generate_store_64(8, mem_start, ("Address for [" + assignment.id + "]"));
                    current_function->instructions.insert(current_function->instructions.end(), store_ins.begin(), store_ins.end());

                    // Add the relative address of the item to the start position of the function in memory to acquire the actual destination
                    current_function->instructions.push_back("\n\tldw r1 $0(ls)\t; Start of stack frame\n");
                    current_function->instructions.push_back("\n\tadd r0 r0 r1\t; Item location in function memory\n");

                    // Get the calculation result off the stack
                    current_function->instructions.push_back("\n\t; ---- Get result ----\n");
                    current_function->instructions.push_back("\tpopw r8 ls\n");

                    // Store the result at the memory address
                    current_function->instructions.push_back("\n\t; ---- Store result ---- \n");
                    current_function->instructions.push_back("\tstw r0(gs) r8\t ; Store in memory\n");
                    break;
                }
                case Intermediate::InstructionSet::USE_RAW:
                {
                    switch(assignment.assignment_classifier)
                    {
                    case Intermediate::AssignmentClassifier::INTEGER:  setup_integer(assignment.id, ins.value);  break;
                    case Intermediate::AssignmentClassifier::DOUBLE:   setup_double(assignment.id, ins.value );  break;
                    case Intermediate::AssignmentClassifier::CHAR:     setup_char(assignment.id, ins.value );    break;
                    default:
                        error_man.report_custom("Codegen", "Developer error: Assignment Classifier switch reached default", true);
                        break;
                    }
                    break;
                }
                case Intermediate::InstructionSet::POW:
                {
                    current_function->instructions.push_back("\n\t; <<< POW >>> \n");
                    current_function->instructions.push_back("\n\tpopw r2 ls \t ; Calculation RHS\n\tpopw r1 ls \t ; Calculation LHS\n");

                    std::string function_name;
                    if(assignment.assignment_classifier == Intermediate::AssignmentClassifier::DOUBLE)
                    {
                        asm_support.import_math(AsmSupport::Math::POW_D, function_name, program_init);

                    }
                    else
                    {
                        asm_support.import_math(AsmSupport::Math::POW_I, function_name, program_init);
                    }

                    current_function->instructions.push_back("\n\tcall " + function_name + " ; Call to perfom power\n\n");
                    current_function->instructions.push_back("\tpushw ls r0\t; Push value to local stack for calculation\n");
                    break;
                }
                case Intermediate::InstructionSet::MOD:
                {
                    current_function->instructions.push_back("\n\t; <<< MOD >>> \n");
                    current_function->instructions.push_back("\n\tpopw r2 ls \t ; Calculation RHS\n\tpopw r1 ls \t ; Calculation LHS\n");

                    std::string function_name;
                    if(assignment.assignment_classifier == Intermediate::AssignmentClassifier::DOUBLE)
                    {
                        asm_support.import_math(AsmSupport::Math::MOD_D, function_name, program_init);
                    }
                    else
                    {
                        asm_support.import_math(AsmSupport::Math::MOD_I, function_name, program_init);
                    }

                    current_function->instructions.push_back("\n\tcall " + function_name + " ; Call to perfom modulus\n\n");
                    current_function->instructions.push_back("\tpushw ls r0\t; Push value to local stack for calculation\n");
                    break;
                }

                /*
                    TODO: Once grammar allows us to include function calls in expressions, this will need to be filled out!
                          There may be MANY bytes that come back, so a new method triggered by this one would be ideal
                
                */
                case Intermediate::InstructionSet::CALL:       error_man.report_custom("Codegen", "Developer: CALL not completed"); break;

                case Intermediate::InstructionSet::RETURN:
                {
                    current_function->build_return();
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
        current_function->instructions.push_back("\n\t; <<< SETUP INT >>> \n");

        // Get the numerical value
        int64_t i_value = std::stoll(value);

        // Conditionally little-endian the thing and convert it to an unsigned value
        uint64_t unsigned_value = ENDIAN::conditional_to_le_64(static_cast<uint64_t>(i_value));

        uint8_t bytes = 8;
        if(i_value < std::numeric_limits<int8_t>::max() && i_value > std::numeric_limits<int8_t>::min()){          bytes = 1; }
        else if (i_value < std::numeric_limits<int16_t>::max() && i_value > std::numeric_limits<int16_t>::min()){  bytes = 2; }
        else if(i_value < std::numeric_limits<int32_t>::max() && i_value > std::numeric_limits<int32_t>::min()) {  bytes = 4; }

        // Generate the store for 64 bit
        std::vector<std::string> store_ins = PARTS::generate_store_64(bytes, unsigned_value, id);

        // Merge store instructions into program init
        current_function->instructions.insert(current_function->instructions.end(), store_ins.begin(), store_ins.end());

        current_function->instructions.push_back("\n\t; ---- Move the value ----\n");
        current_function->instructions.push_back("\tpushw ls r0\t ; Place on local stack for calculation\n");
    }

    // ----------------------------------------------------------
    //
    // ----------------------------------------------------------

    void Codegen::setup_double(std::string id, std::string value)
    {
        current_function->instructions.push_back("\n\t; <<< SETUP REAL >>> \n");

        uint64_t unsigned_value = ENDIAN::conditional_to_le_64(
                                    UTIL::convert_double_to_uint64(std::stod(value)
                                    )
                                );
        
        // Generate the store the real
        std::vector<std::string> store_ins = PARTS::generate_store_64(8, unsigned_value, id);

        // Merge store instructions into program init
        current_function->instructions.insert(current_function->instructions.end(), store_ins.begin(), store_ins.end());

        current_function->instructions.push_back("\n\t; ---- Move the value ----\n");
        current_function->instructions.push_back("\tpushw ls r0\t ; Place on local stack for calculation\n");
    }

    // ----------------------------------------------------------
    //
    // ----------------------------------------------------------

    void Codegen::setup_char(std::string id, std::string value)
    {
        current_function->instructions.push_back("\n\t; <<< SETUP CHAR >>> \n");

        char c_val = value[1];

        uint32_t u_val = static_cast<uint32_t>(c_val);

        current_function->instructions.push_back("\n\tmov r0 $" + std::to_string(u_val) + "\t; Move char into reg\n\n");

        current_function->instructions.push_back("\tpush ls r0\t ; Place on ls so storage method gets it\n");
    }

    // ----------------------------------------------------------
    //
    // ----------------------------------------------------------

    void Codegen::null_return()
    {
        // Tell the function to return without getting information from the stack
        current_function->build_return(false);
    }

    // ----------------------------------------------------------
    //
    // ----------------------------------------------------------

    void Codegen::create_call(Intermediate::Call call)
    {
        current_function->instructions.push_back("\n\t; <<< CALL >>> \n");

        uint16_t idx = 0;
        for(auto & p : call.params)
        {
            std::string r = std::to_string(idx++);

            if(p.is_address)
            {
                current_function->instructions.push_back("\n\tldw r8 $0(ls) \t; Initial stack offset\n");
                current_function->instructions.push_back("\tmov r" + r + " $" + std::to_string(p.value) + " \t ; Load relative address of [" + p.id + "]\n");
                current_function->instructions.push_back("\tadd r" + r + " r" + r + " r8 \t ; Add stack frame location to relative address\n");
            }
            else
            {
                error_man.report_custom("CodeGen", " Raw value loading for function call not yet complete", true);
            }
        }

        current_function->instructions.push_back("\n\tcall " + call.destination + "\n");
    }

}