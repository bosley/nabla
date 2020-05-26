#include "nhll.hpp"

#include <iostream>

namespace NHLL
{
    // --------------------------------------------------------------
    //
    // --------------------------------------------------------------

    void UseStmt::visit(NhllVisitor &visitor)
    {
        visitor.accept(*this);
    }
    
    // --------------------------------------------------------------
    //
    // --------------------------------------------------------------

    void SetStmt::visit(NhllVisitor &visitor)
    {
        visitor.accept(*this);
    }

    // --------------------------------------------------------------
    //
    // --------------------------------------------------------------

    void WhileStmt::visit(NhllVisitor &visitor)
    {
        visitor.accept(*this);
    }

    // --------------------------------------------------------------
    //
    // --------------------------------------------------------------

    void NhllFunction::visit(NhllVisitor &visitor)
    {
        visitor.accept(*this);
    }
}
