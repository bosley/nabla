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
        std::stack<std::string> op;
        std::string res = "";
        std::map<std::string,std::pair<int,int> > ops;
        ops["^"] = std::make_pair(4,1); // E
        ops["*"] = std::make_pair(3,0); // M
        ops["/"] = std::make_pair(3,0); // D
        ops["+"] = std::make_pair(2,0); // A
        ops["-"] = std::make_pair(2,0); // S

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

            if(tmp == "^" || tmp == "*" || tmp == "/" || tmp == "+" || tmp == "-")
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