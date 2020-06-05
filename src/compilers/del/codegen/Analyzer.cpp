#include "Analyzer.hpp"

#include <iostream>
#include <sstream>

namespace DEL
{
    namespace
    {
        template<typename Numeric>
        inline static bool is_number(const std::string& s)
        {
            Numeric n;
            return((std::istringstream(s) >> n >> std::ws).eof());
        }
    }

    // ----------------------------------------------------------
    //
    // ----------------------------------------------------------

    Analyzer::Analyzer(Errors & err, SymbolTable & symbolTable, Codegen & code_gen) : 
                                                                        error_man(err), 
                                                                        symbol_table(symbolTable),
                                                                        code_gen(code_gen)
    {

    }

    // ----------------------------------------------------------
    //
    // ----------------------------------------------------------

    Analyzer::~Analyzer()
    {
        
    }

    // ----------------------------------------------------------
    //
    // ----------------------------------------------------------

    void Analyzer::ensure_unique_symbol(std::string id)
    {
        if(symbol_table.does_symbol_exist(id, true))
        {
            error_man.report_previously_declared(id);
        }
    }

    // ----------------------------------------------------------
    //
    // ----------------------------------------------------------

    void Analyzer::ensure_id_in_current_context(std::string id, std::vector<ValType> allowed)
    {
        // Check symbol table to see if an id exists, don't display information yet
        if(!symbol_table.does_symbol_exist(id, false))
        {
            // Reports the error and true marks the program for death
            error_man.report_unknown_id(id, true);
        }

        // If allowed is empty, we just wanted to make sure the thing existed
        if(allowed.size() == 0)
        {
            return;
        }

        // Ensure type is one of the allowed types
        bool is_allowed = false;
        for(auto & v : allowed)
        {
            if(symbol_table.is_existing_symbol_of_type(id, v))
            {
            is_allowed = true;
            }
        }

        // If the type isn't allowed
        if(!is_allowed)
        {
            error_man.report_unallowed_type(id, true);
        }
    }

    // ----------------------------------------------------------
    //
    // ----------------------------------------------------------

    ValType Analyzer::get_id_type(std::string id)
    {
        ValType t = symbol_table.get_value_type(id);

        if(t == ValType::NONE)
        {
            error_man.report_unknown_id(id, true);
        }

        return t;
    }

    // ----------------------------------------------------------
    //
    // ----------------------------------------------------------

    void Analyzer::build_function(Function *function)
    {
        std::cout << "Analyzer >> Building function : " << function->name << std::endl;

        // Ensure function is unique
        if(symbol_table.does_context_exist(function->name))
        {
            // Dies if not unique
            error_man.report_previously_declared(function->name);
        }

        // Create function context
        // Remove the last function's context
        symbol_table.new_context(function->name, true);

        // Place function parameters into context
        for(auto & p : function->params)
        {
            symbol_table.add_symbol(p.id, p.type);
        }

        // Tell code generator to start function with given parametrs
        code_gen.begin_function(function->name, function->params);

        // Iterate over function elements and visit them with *this
        for(auto & el : function->elements)
        {
            // Visiting elements will trigger analyzer to check particular element
            // for any errors that may be present
            el->visit(*this);

            // If no errors are present in the current statement, then we trigger code generation

            // --- This might be removed in-place of a simplified object being passed to codegen so it doesn't have to parse trees, but idk yet!
          //  el->visit(code_gen);

            // Now that the item is constructed, we free the memory
            delete el;
        }

        // Tell code generator that we are done constructin the current function
        code_gen.end_function();

        // Function is constructed - and elements have been freed
        delete function;
    }

    // -----------------------------------------------------------------------------------------
    // 
    //                              Visitor Methods
    //
    // -----------------------------------------------------------------------------------------

    void Analyzer::accept(Assignment &stmt)
    {
        /*
            If the assignment is a reassignment, it will be indicated via REQ_CHECK. 
            If this is active, we need to ensure it exists in reach and ensure that the type 
            allows us to do assignments. 
        */
        if(stmt.data_type == ValType::REQ_CHECK)
        {
            // Check that the rhs is in context and is a type that we are allowing for assignment
            // NOTE : When functions are allowed in asssignment this will have to be updated <<<<<<<<<<<<<<<<<<
            ensure_id_in_current_context(stmt.lhs, {ValType::INTEGER, ValType::REAL, ValType::CHAR});

            // Now we know it exists, we set the data type to what it states in the map
            stmt.data_type = get_id_type(stmt.lhs);
        }

        /*
            Call the validation method to walk the assignment AST and build an instruction set for the 
            code generator while analyzing the data to ensure that all variables exist and to pull the type
            that the resulting operation would be
        */
        Intermediate::AssignmentClassifier classification = Intermediate::AssignmentClassifier::INTEGER; // Assume int 
        
        std::string postfix_expression = validate_assignment_ast(stmt.rhs, classification, stmt.data_type, stmt.lhs);

        std::cout << "Resulting eq: " << stmt.lhs << " = " << postfix_expression << std::endl;

        /*

            TODO : Need to get address of LHS, or determine if its new and populate the location_info struct below
        
        */
        Intermediate::Location location_info;

        // Generate the instructions for code generation
        Intermediate::Assignment assignment_command = intermediate_rep.encode_postfix_assignment_expression(location_info, postfix_expression);

        // Call into code generation to create assignment ASM
        code_gen.assignment(assignment_command);
    }

    // -----------------------------------------------------------------------------------------
    // 
    //                              Assignment Validation Methods
    //
    // -----------------------------------------------------------------------------------------

    void Analyzer::check_value_is_valid_for_assignment(ValType type_to_check, Intermediate::AssignmentClassifier & c, ValType & et, std::string & id)
    {
        switch(type_to_check)
        {
            case ValType::STRING   :    error_man.report_custom("Analyzer", " STRING found in arithmetic exp",    true); // Grammar should have
            case ValType::REQ_CHECK:    error_man.report_custom("Analyzer", " REQ_CHECK found in arithmetic exp", true); // filteres these out
            case ValType::NONE     :    error_man.report_custom("Analyzer", " NONE found in arithmetic exp",      true);
            case ValType::FUNCTION :    error_man.report_custom("Analyzer", " FUNCTION found in arithmetic exp",  true);
            case ValType::REAL     : 
            {
                // Promote to Double if any double is present
                c = Intermediate::AssignmentClassifier::DOUBLE;

                if((et != ValType::REAL) && (et != ValType::REAL)) { error_man.report_unallowed_type(id, true); }

                break;
            }
            case ValType::INTEGER  :
            {
                // We assume its an integer to start with so we dont set type (because we allow ints inside double exprs)
                if(et != ValType::INTEGER) { error_man.report_unallowed_type(id, true); }
                break;
            }
            case ValType::CHAR     :
            {
                
                c = Intermediate::AssignmentClassifier::CHAR;

                if(et != ValType::CHAR)   { error_man.report_unallowed_type(id, true); } // If Assignee isn't a char, we need to die
                break;
            }

            /*
                These are values of things that will be added in later and will need to be handled once they are created in the grammar

                Structs - LHS of assignment will be required to be a struct of the same type
                Function call - Return type will need to be the same as the LHS of assignment
                Ptr - Will need to check that LHS is also a pointer
                Ref - Will need to ensure that LHS is the same type as the reference value and that the reference exists
            */
            case ValType::STRUCT        : error_man.report_custom("Analyzer::DEV", "An item not yet create has shown up in the assignment analyzer", true);
            case ValType::FUNCTION_CALL : error_man.report_custom("Analyzer::DEV", "An item not yet create has shown up in the assignment analyzer", true);
            case ValType::PTR           : error_man.report_custom("Analyzer::DEV", "An item not yet create has shown up in the assignment analyzer", true); 
            case ValType::REF           : error_man.report_custom("Analyzer::DEV", "An item not yet create has shown up in the assignment analyzer", true);
        }
    }

    // ----------------------------------------------------------
    // Assignee's expected type abbreviated to 'et' 
    // ----------------------------------------------------------

    std::string Analyzer::validate_assignment_ast(AST * ast, Intermediate::AssignmentClassifier & c, ValType & et, std::string & id)
    {
        switch(ast->node_type)
        {
            case NodeType::ID  : 
            { 
                // Ensure the ID is within current context. Allowing any type
                ensure_id_in_current_context(ast->value, {});

                // Check for promotion
                ValType id_type = get_id_type(ast->value);

                // Make sure that the known value of the identifier is one valid given the current assignemnt
                check_value_is_valid_for_assignment(id_type, c, et, id);

                /*
                        TODO : 

                                Need to encode ID information into the returned string so the Intermediate class
                                can determine what information to encode for the code generator
            
                */

                return ast->value;
            }
            case NodeType::ADD    :return (validate_assignment_ast(ast->l, c, et, id) + " " + validate_assignment_ast(ast->r, c, et, id) + " + "  );  
            case NodeType::SUB    :return (validate_assignment_ast(ast->l, c, et, id) + " " + validate_assignment_ast(ast->r, c, et, id) + " - "  );
            case NodeType::DIV    :return (validate_assignment_ast(ast->l, c, et, id) + " " + validate_assignment_ast(ast->r, c, et, id) + " / "  );
            case NodeType::MUL    :return (validate_assignment_ast(ast->l, c, et, id) + " " + validate_assignment_ast(ast->r, c, et, id) + " * "  );
            case NodeType::MOD    :return (validate_assignment_ast(ast->l, c, et, id) + " " + validate_assignment_ast(ast->r, c, et, id) + " % "  );
            case NodeType::POW    :return (validate_assignment_ast(ast->l, c, et, id) + " " + validate_assignment_ast(ast->r, c, et, id) + " ^ "  );
            case NodeType::LTE    :return (validate_assignment_ast(ast->l, c, et, id) + " " + validate_assignment_ast(ast->r, c, et, id) + " <= "  );
            case NodeType::GTE    :return (validate_assignment_ast(ast->l, c, et, id) + " " + validate_assignment_ast(ast->r, c, et, id) + " >= "  );
            case NodeType::GT     :return (validate_assignment_ast(ast->l, c, et, id) + " " + validate_assignment_ast(ast->r, c, et, id) + " > "  );
            case NodeType::LT     :return (validate_assignment_ast(ast->l, c, et, id) + " " + validate_assignment_ast(ast->r, c, et, id) + " < "  );
            case NodeType::EQ     :return (validate_assignment_ast(ast->l, c, et, id) + " " + validate_assignment_ast(ast->r, c, et, id) + " == "  );
            case NodeType::NE     :return (validate_assignment_ast(ast->l, c, et, id) + " " + validate_assignment_ast(ast->r, c, et, id) + " != "  );
            case NodeType::LSH    :return (validate_assignment_ast(ast->l, c, et, id) + " " + validate_assignment_ast(ast->r, c, et, id) + " << " );
            case NodeType::RSH    :return (validate_assignment_ast(ast->l, c, et, id) + " " + validate_assignment_ast(ast->r, c, et, id) + " >> " );
            case NodeType::BW_OR  :return (validate_assignment_ast(ast->l, c, et, id) + " " + validate_assignment_ast(ast->r, c, et, id) + " or " );
            case NodeType::BW_XOR :return (validate_assignment_ast(ast->l, c, et, id) + " " + validate_assignment_ast(ast->r, c, et, id) + " xor ");
            case NodeType::BW_AND :return (validate_assignment_ast(ast->l, c, et, id) + " " + validate_assignment_ast(ast->r, c, et, id) + " and ");
            case NodeType::OR     :return (validate_assignment_ast(ast->l, c, et, id) + " " + validate_assignment_ast(ast->r, c, et, id) + " || " );
            case NodeType::AND    :return (validate_assignment_ast(ast->l, c, et, id) + " " + validate_assignment_ast(ast->r, c, et, id) + " && " );
            case NodeType::BW_NOT :return (validate_assignment_ast(ast->l, c, et, id) + " not ");
            case NodeType::NEGATE :return (validate_assignment_ast(ast->l, c, et, id) + " ! "  );
            case NodeType::ROOT   : error_man.report_custom("Analyzer", "ROOT NODE found in arithmetic exp", true);
            case NodeType::VAL : 
            { 
                // Check that the raw value is one that is valid within the current assignment
                check_value_is_valid_for_assignment(ast->val_type, c, et, id);
                return ast->value;
            }
            default:
            return "Its dead, jim";
        }
        return "Complete";
    }
}