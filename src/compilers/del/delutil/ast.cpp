#include "Ast.hpp"

namespace DEL
{
    void Assignment::visit(Visitor &visitor)
    {
        visitor.accept(*this);
    }
}