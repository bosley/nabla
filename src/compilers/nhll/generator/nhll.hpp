#ifndef NHLL_LANGUAGE_HPP
#define NHLL_LANGUAGE_HPP

#include <string>
#include <vector>
#include <memory>

namespace NHLL
{
    class NhllVisitor;

    class NhllElement
    {
    public:
        virtual ~NhllElement() = default;
        virtual void visit(NhllVisitor &visitor) = 0;
    };

    enum class Conditionals
    {
        LT, GT, LTE, GTE, NE, EQ, NONE
    };

    enum class ConditialExpressionType
    {
        ID, INT, REAL, EXPR
    };

    class ConditionalExpression
    {
    public:
        ConditionalExpression(){}
        ConditionalExpression(ConditialExpressionType type, 
                              Conditionals condition, 
                              std::string expression_lhs,
                              std::string expression_rhs) : type(type),
                                                            cond(condition),
                                                            lhs(expression_lhs),
                                                            rhs(expression_rhs)
        {
            
        }
        ConditialExpressionType type;
        Conditionals cond;
        std::string lhs;
        std::string rhs;
    };

    //
    //
    //
    class UseStmt : public NhllElement
    {
    public:
        UseStmt() {}

        UseStmt(std::string module, std::string as_name) 
                            : module(module),
                              as_name(as_name){}

        UseStmt(UseStmt *o) : module(o->module),
                              as_name(o->as_name){}

        virtual void visit(NhllVisitor &visitor) override;

        std::string module;
        std::string as_name;
    };

    //
    //
    //
    class SetStmt : public NhllElement
    {
    public:
        SetStmt() {}

        SetStmt(std::string lhs, std::string exp) 
                            : identifier(lhs),
                              expression(exp){}

        SetStmt(SetStmt *o) : identifier(o->identifier),
                              expression(o->expression){}

        virtual void visit(NhllVisitor &visitor) override;

        std::string identifier;
        std::string expression;
    };

    //
    //
    //
    class WhileStmt : public NhllElement
    {
    public:
        WhileStmt() {}

        WhileStmt(ConditionalExpression *expr,
                  std::vector<std::shared_ptr<NHLL::NhllElement>> el)
        {
            condition.type = expr->type;
            condition.cond = expr->cond;
            condition.lhs  = expr->lhs ;
            condition.rhs  = expr->rhs ;

            elements = el;
        }

        WhileStmt(WhileStmt *o)
        {
            condition.type = o->condition.type;
            condition.cond = o->condition.cond;
            condition.lhs  = o->condition.lhs ;
            condition.rhs  = o->condition.rhs ;
            elements = o->elements;
        }

        virtual void visit(NhllVisitor &visitor) override;

        ConditionalExpression condition;
        std::vector<std::shared_ptr<NHLL::NhllElement>> elements;
    };

    //
    //
    //
    class NhllFunction
    {
    public:
        NhllFunction();
        ~NhllFunction();
        //bool hasParams;
        //std::vector<NhllElement *> statements;
    };

    /*
            Visitor is last so we don't  have to fwd decl all the things
    */
    class NhllVisitor
    {
    public:
        virtual void accept(UseStmt &stmt) = 0;
        virtual void accept(SetStmt &stmt) = 0;
        virtual void accept(WhileStmt &stmt) = 0;
    };
}

#endif