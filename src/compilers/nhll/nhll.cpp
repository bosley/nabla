#include "nhll.hpp"

#include <iostream>

namespace NHLL
{    
    // --------------------------------------------------------------
    //
    // --------------------------------------------------------------

    void LetStmt::visit(NhllVisitor &visitor)
    {
        visitor.accept(*this);
    }

    // --------------------------------------------------------------
    //
    // --------------------------------------------------------------

    void ReAssignStmt::visit(NhllVisitor &visitor)
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

    void LoopStmt::visit(NhllVisitor &visitor)
    {
        visitor.accept(*this);
    }

    // --------------------------------------------------------------
    //
    // --------------------------------------------------------------

    void BreakStmt::visit(NhllVisitor &visitor)
    {
        visitor.accept(*this);
    }


    // --------------------------------------------------------------
    //
    // --------------------------------------------------------------

    void CallStmt::visit(NhllVisitor &visitor)
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
