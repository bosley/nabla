#ifndef NHLL_LANGUAGE_HPP
#define NHLL_LANGUAGE_HPP

#include <string>

namespace NHLL
{
    class NhllVisitor;

    class NhllElement
    {
    public:
        virtual ~NhllElement() = default;
        virtual void visit(NhllVisitor &visitor) = 0;
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
    };
}

#endif