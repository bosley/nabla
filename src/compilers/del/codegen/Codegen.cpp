#include "Codegen.hpp"

#include <iostream>
#include <vector>

namespace DEL
{

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
        std::cout << "CODEGEN : Assignement!" << std::endl;

        /*
            assignment.id                    -> The name of the thing we are assigning for comments
            assignemnt.memory_info           -> Where we need to store the thing
            assignment.assignment_classifier -> How to treat the given data (int, char, real)
            assignment.instructions          -> What to do to the data in RPN form
        */

       std::vector<std::string> lines;

       std::string remove_words_for_calc = "\tpopw r9 ls \t ; Calculation RHS\n\tpopw r8 ls \t ; Calculation LHS\n";

       std::string remove_single_word_for_calc = "\tpopw r8 ls \t ; Calculation LHS\n";

       std::string calculation_chunk = "r8 r8 r9 \t ; Perform operation, store in r8\n\tpushw r8 ls \t ; Put result into ls\n";

       std::string calculate_unary = "r8 r8 \t ; Perform unary operation, store in r8\n\tpushw r8 ls \t ; Put result into ls\n";

       bool is_double_variant = (assignment.assignment_classifier == Intermediate::AssignmentClassifier::DOUBLE);

       for(auto & ins : assignment.instructions)
       {
           switch(ins.instruction)
           {
                /*
                
                        TODO : These need to be filled out
                
                */
                case Intermediate::InstructionSet::ADD:     lines.push_back(remove_words_for_calc + ((is_double_variant) ? "\tadd.d " : "\tadd ") + calculation_chunk); break;
                case Intermediate::InstructionSet::SUB:     lines.push_back(remove_words_for_calc + ((is_double_variant) ? "\tsub.d " : "\tsub ") + calculation_chunk); break;
                case Intermediate::InstructionSet::DIV:     lines.push_back(remove_words_for_calc + ((is_double_variant) ? "\tdiv.d " : "\tdiv ") + calculation_chunk); break;
                case Intermediate::InstructionSet::MUL:     lines.push_back(remove_words_for_calc + ((is_double_variant) ? "\tmul.d " : "\tmul ") + calculation_chunk); break;
                case Intermediate::InstructionSet::RSH:     lines.push_back(remove_words_for_calc + ( "\tlsh " + calculation_chunk)); break;
                case Intermediate::InstructionSet::LSH:     lines.push_back(remove_words_for_calc + ( "\trsh " + calculation_chunk)); break;
                case Intermediate::InstructionSet::BW_OR:   lines.push_back(remove_words_for_calc + ( "\tor " + calculation_chunk )); break;
                case Intermediate::InstructionSet::BW_NOT:  lines.push_back(remove_single_word_for_calc + ( "\tnot " + calculate_unary)); break;
                case Intermediate::InstructionSet::BW_XOR:  lines.push_back(remove_words_for_calc + ( "\txor " + calculation_chunk)); break;
                case Intermediate::InstructionSet::BW_AND:  lines.push_back(remove_words_for_calc + ( "\tand " + calculation_chunk)); break;
                case Intermediate::InstructionSet::LTE:
                case Intermediate::InstructionSet::LT:
                case Intermediate::InstructionSet::GTE:
                case Intermediate::InstructionSet::GT:
                case Intermediate::InstructionSet::EQ:
                case Intermediate::InstructionSet::NE:
                case Intermediate::InstructionSet::OR:
                case Intermediate::InstructionSet::AND:
                case Intermediate::InstructionSet::NEGATE:
                case Intermediate::InstructionSet::LOAD_BYTE:
                case Intermediate::InstructionSet::STORE_BYTE:
                case Intermediate::InstructionSet::LOAD_WORD:
                case Intermediate::InstructionSet::STORE_WORD:
                case Intermediate::InstructionSet::CALL:
                case Intermediate::InstructionSet::GET_RESULT:
                case Intermediate::InstructionSet::USE_RAW:
                {
                    /*
                        Need to come up with a means to hot-load doubles. - Maybe just declare with .double and store its address

                        Need to take this current value and load it into the thing

                        If its a double, and we use .double we have to account for the offeset to the global stack
                    
                    */
                    break;
                }
                case Intermediate::InstructionSet::POW:
                {
                    if(!has_included_math_functions){ add_build_in_math_functions(); }

                    // TODO 
                    
                    // Need to load things into registers used for power function
                    // Call that function
                    // Get the resul and store 
                    break;
                }
                case Intermediate::InstructionSet::MOD:
                {
                    if(!has_included_math_functions){ add_build_in_math_functions(); }

                    // TODO 

                    // Need to load things into registers used for power function
                    // Call that function
                    // Get the resul and store 
                    break;
                }
                default:
                    error_man.report_custom("Codegen", "Developer error : Default accessed in assignment", true);
                    break;
           }
       }



       std::cout << std::endl;

       for(auto & l : lines)
       {
           std::cout << l;
       }
    }
}