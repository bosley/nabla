/*
    This postfix conversion method was adapted from one I wrote a long time ago. I'm not proud of it. It works well, 
    but the implementation is aweful. Low hanging fruit for anyone who wants to make this better. 
*/

#include "nhll_postfix.hpp"

#include <map>
#include <stack>
#include <sstream>

namespace NHLL
{
    std::vector<Postfix::Element> Postfix::convert(std::string exp)
    {
        // https://en.wikipedia.org/wiki/Order_of_operations

        std::stack<std::string> op;
        std::string res = "";
        std::map<std::string,std::pair<int,int> > ops;
        ops["^"]  = std::make_pair(9,1); // E
        ops["*"]  = std::make_pair(8,0); // M
        ops["/"]  = std::make_pair(8,0); // D
        ops["%"]  = std::make_pair(8,0); // MOD
        ops["+"]  = std::make_pair(7,0); // A
        ops["-"]  = std::make_pair(7,0); // S
        ops["~"]  = std::make_pair(7,0); // S
        ops["<"]  = std::make_pair(6,0); // left shift
        ops[">"]  = std::make_pair(6,0); // right shift
        ops["&"]  = std::make_pair(5,0); // and
        ops["x"]  = std::make_pair(4,0); // xor
        ops["|"]  = std::make_pair(3,0); // or

        std::vector<Element> element_vector;

        for(long unsigned int i = 0; i < exp.size(); i++)
        {
            std::string c_str = std::string(1, exp[i]);

            if(exp[i] == '(')
            {
                op.push(c_str);
            }
            else if(exp[i] == ')')
            {
                while(op.top() != "(")
                {
                        res += " " + op.top() + " ";
                        op.pop();
                }
                op.pop();
            }
            else if(ops.find(c_str) != ops.end())
            {
                while(!op.empty() &&
                        ((ops[c_str].second == 0 &&
                            ops[op.top()].first == ops[c_str].first) ||
                            ops[c_str].first < ops[op.top()].first))
                {
                        res += " " + op.top() + " ";
                        op.pop();
                }
                res += " ";
                op.push(c_str);
            }
            else
            {
                res += exp[i];
            }
        }
        while(!op.empty())
        {
                res += " " + op.top() + " ";
                op.pop();
        }

        // Build the element vector
        std::string tmp;
        std::stringstream ss(res);
        while (ss >> tmp)
        {
            // Sanity check
            if(tmp.size() == 0)
            {
                continue;
            }

            if(tmp == "^" || tmp == "*" || tmp == "/" || tmp == "+" || 
               tmp == "-" || tmp == "<" || tmp == ">" || tmp == "|" ||
               tmp == "x" || tmp == "&" || tmp == "%" || tmp == "~")
            {
                element_vector.push_back(Element{ tmp, Type::OP });
            }
            else
            {
                element_vector.push_back(Element{ tmp, Type::VALUE });
            }
        }
        return element_vector;
    }
}