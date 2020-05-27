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