#ifndef NHLL_VISITOR_HPP
#define NHLL_VISITOR_HPP

namespace NABLA
{
    class Nhll_Use;

    class NhllVisitor
    {
    public:
        void visit(Nhll_Use & el);
    };

    class Element
    {
    public:
        virtual void accept(NhllVisitor & visitor) = 0;
    };

    /*
        use("something")
        use("soemthing", "newname")
    */
    class Nhll_Use : public Element
    {
    public:
        virtual void accept(NhllVisitor & visitor) override
        {
            visitor.visit(*this);
        }
    };
}

#endif