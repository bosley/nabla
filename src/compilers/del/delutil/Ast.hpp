#ifndef DEL_AST_HPP
#define DEL_AST_HPP

#include "types.hpp"
#include <vector>

namespace DEL
{
    //
    //  A node
    //
    class Node
    {
    public:
        Node(NodeType type) : node_type(type), val_type(ValType::NONE) {}

        NodeType node_type;
        ValType val_type;
        std::string value;
    };

    //
    //  A tree
    //
    class AST : public Node 
    {
    public: 
        AST() : Node(NodeType::ROOT), l(nullptr), r(nullptr) {}
        AST(NodeType type, AST*lhs, AST*rhs) : Node(type),
                                               l(lhs), 
                                               r(rhs) {}

        AST(NodeType type, AST*lhs, AST*rhs, ValType v, std::string a): Node(type),
                                               l(lhs), 
                                               r(rhs) 
        {
            this->val_type = v;
            this->value = a;
        }

        AST * l;
        AST * r;
    };

    //
    // Fwd for a visitor
    //
    class Visitor;

    //
    // A visitable element
    //
    class Element
    {
    public:
        virtual ~Element() = default;
        virtual void visit(Visitor &visitor) = 0;
    };

    // A list of elements
    typedef std::vector<Element*> ElementList;

    //
    //  An assignment
    //
    class Assignment : public Element
    {
    public:
        Assignment(ValType type, std::string lhs, AST * rhs) : data_type(type),
                                                                lhs(lhs),
                                                                rhs(rhs){}
        virtual void visit(Visitor &visit) override;

        ValType data_type;
        std::string lhs;
        AST * rhs;
    };

    //
    //  A return statement
    //
    class ReturnStmt : public Element
    {
    public:
        ReturnStmt(AST * rhs) : data_type(ValType::REQ_CHECK), rhs(rhs){}
        ReturnStmt() : data_type(ValType::NONE), rhs(nullptr){}

        virtual void visit(Visitor &visit) override;

        ValType data_type;
        AST * rhs;
    };

    //
    //  A call 
    //
    class Call : public Element // , public Node // We need to make this a node for call assignments
    {
    public:
        Call(std::string name, std::vector<FunctionParam> params) : name(name), params(params) {}

        virtual void visit(Visitor &visit) override;

        std::string name;
        std::vector<FunctionParam> params;
    };

    //
    //  A function
    //
    class Function
    {
    public:
        Function(std::string name, std::vector<FunctionParam> params, ValType return_type, ElementList elements) :
            name(name), params(params), return_type(return_type), elements(elements){}

        std::string name;
        std::vector<FunctionParam> params;
        ValType return_type;
        ElementList elements;
    };

    //
    //  Visitor 
    //
    class Visitor
    {
    public:
        virtual void accept(Assignment &stmt) = 0;
        virtual void accept(ReturnStmt &stmt) = 0;
        virtual void accept(Call       &stmt) = 0;
    };


}

#endif