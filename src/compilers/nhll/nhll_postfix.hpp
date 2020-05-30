#ifndef NHLL_POSTFIX_HPP
#define NHLL_POSTFIX_HPP

#include <string>
#include <vector>

namespace NHLL
{
    //! \brief Infix to postfix converter
    class Postfix
    {
    public:

        //!\brief A Type for a converted element
        enum class Type
        {
            OP, VALUE
        };

        // The postfixer uses its own symbols in a couple of cases for operations
        // That wouldn't make sense to 'outsiders' so these are defined for them to
        // be called by name
        static constexpr char ADD  = '+';
        static constexpr char MUL  = '*';
        static constexpr char SUB  = '-';
        static constexpr char DIV  = '/';
        static constexpr char POW  = '^';
        static constexpr char MOD  = '%';
        static constexpr char NOT  = '~';
        static constexpr char LSH  = '<';
        static constexpr char RSH  = '>';
        static constexpr char OR   = '|';
        static constexpr char XOR  = 'x';
        static constexpr char AND  = '&';

        //!\brief An element of an expression
        struct Element
        {
            std::string value; // Value
            Type type;         // Element type
        };

        //!\brief Convert an expression to postfix
        //!\param expression The infix expression to conver
        //!\retval Vector of elements. Each element will either be a value (int/double/variable) or an operation (+-/*^)
        //!\note This method ASSUMES that the expression is infix and properly formed. Garbage in will give you garbage out
        std::vector<Element> convert(std::string expression);

    };
}

#endif