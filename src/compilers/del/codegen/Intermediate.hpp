#ifndef DEL_INTERMEDIATE_HPP
#define DEL_INTERMEDIATE_HPP

#include <stdint.h>
#include <string>
#include <vector>

namespace DEL
{
    class Intermediate
    {
    public:
        Intermediate();
        ~Intermediate();

        enum class AssignmentClassifier
        {
            INTEGER, DOUBLE, CHAR, FUNCTION //, STRUCT, STRING
        };

        enum class LocationClassifier
        {
            NEW,
            ADDRESS
        };

        enum class ArithmeticInstruction
        {
            // Native
            ADD, ADD_D, SUB, SUB_D, DIV, DIV_D, 
            MUL, MUL_D, RSH, LSH, AND, OR, NOT, XOR,

            // Built-in
            POW 
        };

        struct Location
        {
            LocationClassifier classification;
            uint64_t address;
        };

        struct AssignemntInstruction
        {
            Intermediate::ArithmeticInstruction instruction;
            std::string value;
        };
        
        struct Assignment
        {
            std::string id;
            Location assignment_location;
            AssignmentClassifier assignment_classifier;
            std::vector<AssignemntInstruction> instructions;
        };

        Assignment encode_postfix_assignment_expression(Location location, std::string expression);
    };
}

#endif