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

    typedef std::vector<NHLL::NhllElement*> ElementList;


    enum class Conditionals
    {
        LT, GT, LTE, GTE, NE, EQ, NONE
    };

    enum class ConditialExpressionType
    {
        ID, INT, REAL, EXPR
    };

    enum class DataPrims
    {
        INT, REAL, STR, NIL
    };

    static std::string DataPrims_to_string(DataPrims dp)
    {
         switch(dp)
         {
            case DataPrims::INT:  return "int"; 
            case DataPrims::REAL: return "real";
            case DataPrims::STR:  return "str"; 
            case DataPrims::NIL:  return "nil"; 
            default: break;
         }
        return "Invalid data prim given for to_string";
    }

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
    class LetStmt : public NhllElement
    {
    public:
        LetStmt() {}

        LetStmt(std::string lhs, std::string rhs, bool is_expr) 
                            : identifier(lhs),
                              set_to(rhs),
                              is_expr(is_expr){}

        LetStmt(LetStmt *o) : identifier(o->identifier),
                              set_to(o->set_to),
                              is_expr(o->is_expr){}

        virtual void visit(NhllVisitor &visitor) override;

        std::string identifier;
        std::string set_to;
        bool is_expr;
    };

    //
    //
    //
    class CallStmt : public NhllElement
    {
    public:
        CallStmt() {}

        CallStmt(std::string function, 
                 std::vector< std::string> params) 
                            : function(function),
                              params(params){}

        CallStmt(CallStmt *o) : function(o->function),
                                params(o->params){}

        virtual void visit(NhllVisitor &visitor) override;

        std::string function;
        std::vector< std::string> params;
    };

    //
    //
    //
    class ReAssignStmt : public NhllElement
    {
    public:
        ReAssignStmt() {}

        ReAssignStmt(std::string lhs, std::string rhs, bool is_expr) 
                            : identifier(lhs),
                              set_to(rhs),
                              is_expr(is_expr){}

        ReAssignStmt(ReAssignStmt *o) : identifier(o->identifier),
                              set_to(o->set_to),
                              is_expr(o->is_expr){}

        virtual void visit(NhllVisitor &visitor) override;

        std::string identifier;
        std::string set_to;
        bool is_expr;
    };

    //
    //
    //
    class LoopStmt : public NhllElement
    {
    public:
        LoopStmt() {}
        LoopStmt(std::string id, ElementList el) : id(id), elements(el){}
        LoopStmt(LoopStmt * o) : id(o->id), elements(o->elements) {}

        virtual void visit(NhllVisitor &visitor) override;

        std::string id;
        ElementList elements;
    };

    //
    //
    //
    class BreakStmt : public NhllElement
    {
    public:
        BreakStmt() {}
        BreakStmt(std::string id) : id(id) {}
        BreakStmt(BreakStmt *  o) : id(o->id){}

        virtual void visit(NhllVisitor &visitor) override;

        std::string id;
    };

    //
    //
    //
    class WhileStmt : public NhllElement
    {
    public:
        WhileStmt() {}

        WhileStmt(ConditionalExpression *expr, ElementList el)
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
        ElementList elements;
    };

    struct FunctionParam
    {
        DataPrims type;
        std::string name;
    };

    //
    //
    //
    class NhllFunction : public NhllElement
    {
    public:
        NhllFunction() {}

        NhllFunction(std::string name, 
                     std::vector<FunctionParam> params,
                     DataPrims return_type,
                     ElementList el) 
                            : name(name),
                              params(params),
                              return_type(return_type),
                              elements(el){}

        NhllFunction(NhllFunction *o) : name(o->name),
                                        params(o->params),
                                        return_type(o->return_type),
                                        elements(o->elements){}

        virtual void visit(NhllVisitor &visitor) override;

        std::string name;
        std::vector<FunctionParam> params;
        DataPrims return_type;
        ElementList elements;
    };

    /*
            Visitor is last so we don't  have to fwd decl all the things
    */
    class NhllVisitor
    {
    public:
        virtual void accept(UseStmt &stmt) = 0;
        virtual void accept(LetStmt &stmt) = 0;
        virtual void accept(ReAssignStmt & stmt) = 0;
        virtual void accept(WhileStmt &stmt) = 0;
        virtual void accept(LoopStmt &stmt) = 0;
        virtual void accept(BreakStmt &stmt) = 0;
        virtual void accept(CallStmt &stmt) = 0;
        virtual void accept(NhllFunction &stmt) = 0;
    };
}

#endif