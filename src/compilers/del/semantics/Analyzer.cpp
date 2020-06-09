#include "Analyzer.hpp"

#include <iostream>
#include <regex>
#include <sstream>

namespace DEL
{
    // ----------------------------------------------------------
    //
    // ----------------------------------------------------------

    Analyzer::Analyzer(Errors & err, SymbolTable & symbolTable, Codegen & code_gen, Memory & memory) : 
                                                                        error_man(err), 
                                                                        symbol_table(symbolTable),
                                                                        code_gen(code_gen),
                                                                        memory_man(memory),
                                                                        endecoder(memory_man),
                                                                        intermediate_rep(symbolTable, memory)
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

        if(function->params.size() > 7)
        {
            error_man.report_custom("Analyzer::build_function", " Given function exceeds current limit of '7' parameters", true);
        }

        // Create function context
        // Don't remove previous context.. we clear the variables out later
        symbol_table.new_context(function->name, false );

        // Place function parameters into context
        for(auto & p : function->params)
        {
            symbol_table.add_symbol(p.id, p.type);
        }

        // Add parameters to the context
        symbol_table.add_parameters_to_current_context(function->params);

        // Add return type to the context
        symbol_table.add_return_type_to_current_context(function->return_type);

        // Tell code generator to start function with given parametrs
        code_gen.begin_function(function->name, function->params);

        // So elements can access function information as we visit them
        current_function = function;

        // Keep an eye out for pieces that we enforce in a function
        function_watcher.has_return = false;

        // Iterate over function elements and visit them with *this
        for(auto & el : function->elements)
        {
            // Visiting elements will trigger analyzer to check particular element
            // for any errors that may be present, and then analyzer will ask Intermediate to
            // generate instructions for the Codegen / Send the instructions to code gen
            el->visit(*this);

            // Now that the item is constructed, we free the memory
            delete el;
        }

        // Tell code generator that we are done constructin the current function
        code_gen.end_function();

        // Clear the symbol table for the given function so elements cant be accessed externally
        // We dont delete the context though, that way can confirm existence later
        symbol_table.clear_existing_context(function->name);

        if(!function_watcher.has_return)
        {
            error_man.report_no_return(function->name);
        }

        current_function = nullptr;

        // Reset the memory manager for alloc variables in new space
        memory_man.reset();

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


        Memory::MemAlloc memory_info;

        bool requires_allocation_in_symbol_table = true;
        if(stmt.data_type == ValType::REQ_CHECK)
        {
            // Check that the rhs is in context and is a type that we are allowing for assignment
            // NOTE : When functions are allowed in asssignment this will have to be updated <<<<<<<<<<<<<<<<<<
            ensure_id_in_current_context(stmt.lhs, {ValType::INTEGER, ValType::REAL, ValType::CHAR});

            // Now we know it exists, we set the data type to what it states in the map
            stmt.data_type = get_id_type(stmt.lhs);

            // We already checked symbol table if this exists, and symbol table is what 
            // handles allocation of memory for the target, so this is safe
            memory_info = memory_man.get_mem_info(stmt.lhs);

            requires_allocation_in_symbol_table = false;
        }
        else
        {
            // If this isn't a reassignment, we need to ensure that the value is unique
            ensure_unique_symbol(stmt.lhs);
        }

        /*
            Call the validation method to walk the assignment AST and build an instruction set for the 
            code generator while analyzing the data to ensure that all variables exist and to pull the type
            that the resulting operation would be
        */

        Intermediate::AssignmentClassifier classification = Intermediate::AssignmentClassifier::INTEGER; // Assume int 
        
        std::string postfix_expression = validate_assignment_ast(stmt.rhs, classification, stmt.data_type, stmt.lhs);

        std::cout << "Resulting eq: " << stmt.lhs << " = " << postfix_expression << std::endl;

        // The unique value doesn't exist yet and needs some memory allocated and 
        // needs to be added to the symbol table
        if(requires_allocation_in_symbol_table)
        {
            // If this fails, we all fail. add_symbol will figure out data size 
            // and add to memory manager (Until we build more complicated structs
            // then we will have to update this call ) <<<<<<<<<<<<<<<<<<<<<<<<, TODO
            symbol_table.add_symbol(stmt.lhs, stmt.data_type);

            // No longer requires allocation
            requires_allocation_in_symbol_table = false;

            // Retrieve the generated memory information
            memory_info = memory_man.get_mem_info(stmt.lhs);
        }

        // Generate the instructions for code generation
        Intermediate::Assignment assignment_command = intermediate_rep.encode_postfix_assignment_expression(memory_info, classification, postfix_expression);

        assignment_command.id = stmt.lhs;

        // Call into code generation to create assignment ASM
        code_gen.assignment(assignment_command);
    }

    // ----------------------------------------------------------
    //
    // ----------------------------------------------------------

    void Analyzer::accept(ReturnStmt & stmt)
    {
        // Create a 'variable assignment' for the return so we can copy the value or whatever
        std::string variable_for_return = symbol_table.generate_unique_return_symbol();

        function_watcher.has_return = true;

        // Handle NIL / NONE Return
        if(stmt.data_type == ValType::NONE)
        {
            code_gen.null_return();
            return;
        }

        // Create an assignment for the return, this will execute the return withing code gen as we set a RETURN node type that is processed by the assignment
        Assignment * return_assignment = new Assignment(current_function->return_type, variable_for_return, new DEL::AST(DEL::NodeType::RETURN, stmt.rhs, nullptr));

        this->accept(*return_assignment);

        delete return_assignment;
    }

    // ----------------------------------------------------------
    // This is a call statment on its own, not in an expression
    // ----------------------------------------------------------

    void Analyzer::accept(Call & stmt)
    {
        validate_call(stmt);

        ValType callee_type = symbol_table.get_return_type_of_context(stmt.name);

        if(callee_type != ValType::NONE)
        {
            error_man.report_calls_return_value_unhandled(current_function->name, stmt.name);
        }

        //code_gen.create_call(intermediate_rep.create_call(stmt.name, stmt.params));
    }

    // -----------------------------------------------------------------------------------------
    // 
    //                              Validation Methods
    //
    // -----------------------------------------------------------------------------------------

    void Analyzer::validate_call(Call & stmt)
    {
        // Disallow recursion until it is handled
        if(stmt.name == current_function->name)
        {
            error_man.report_custom("Analyzer", "Function recursion has not yet been implemented in Del", true);
        }

        // Ensure that the called method exists
        if(!symbol_table.does_context_exist(stmt.name))
        {
            error_man.report_callee_doesnt_exist(stmt.name);
        }

        // Get the callee params
        std::vector<FunctionParam> callee_params = symbol_table.get_context_parameters(stmt.name);

        // Ensure number of params match
        if(stmt.params.size() != callee_params.size())
        {
            error_man.report_mismatched_param_length(current_function->name, stmt.name, callee_params.size(), stmt.params.size());
        }

        // Ensure all paramters exist
        for(auto & p : stmt.params)
        {
            // If we need to get the type, get the type now that we know it exists
            if(p.type == ValType::REQ_CHECK)
            {
                // Ensure the thing exists, because REQ_CHECK dictates that the parameter is a variable, not a raw
                if(!symbol_table.does_symbol_exist(p.id))
                {
                    std::cerr << "Paramter in call to \"" << stmt.name << "\" does not exist in the current context" << std::endl;
                    error_man.report_unknown_id(p.id, true);
                }

                p.type = get_id_type(p.id);
            }
        }

        // Check that the param types match
        for(uint16_t i = 0; i < stmt.params.size(); i++ )
        {
            if(stmt.params[i].type != callee_params[i].type)
            {
                std::cerr << "Parameter \"" << stmt.params[i].id << "\" does not match expected type in paramter list of function \"" << stmt.name << "\"" << std::endl;
                error_man.report_unallowed_type(stmt.params[i].id, true);
            }
        }
    }

    // ----------------------------------------------------------
    //
    // ----------------------------------------------------------

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

                if((et != ValType::REAL) && (et != ValType::REAL)) 
                {
                    std::string error_message = id;

                    // There are better ways to do this, but if it happens at all it will only happen once during the compiler run
                    // as we are about to die
                    if(std::regex_match(id, std::regex("(__return__assignment__).*")))
                    {
                        error_message = "Function (" + current_function->name + ")";
                    } 
                    error_man.report_unallowed_type(error_message, true); 
                }

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
        }
    }

    // ----------------------------------------------------------
    // Assignee's expected type abbreviated to 'et' 
    // ----------------------------------------------------------

    std::string Analyzer::validate_assignment_ast(AST * ast, Intermediate::AssignmentClassifier & c, ValType & et, std::string & id)
    {
        /*
            Note:
                I know that using the tree to build a big string expression that is tokenized and parsed again isn't ideal. This is my first 
                venture into using ASTs and I thought it would make it easier on me to have an RPN expression that I can build instructions for
                the code generator around. Once the code generator is completed, and optimized as-per the note at the top of the Codegen.cpp
                file, then I would like to revisit this to see what I can do about making this better
        */
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

                // Get the memory address of the ID
                Memory::MemAlloc mem_info = memory_man.get_mem_info(ast->value);

                // Build a string for the intermediate layer to determine what to load and how much to load
                return "#ID:" + std::to_string(mem_info.start_pos) + ":" + std::to_string(mem_info.bytes_alloced);
            }
            
            case NodeType::CALL :
            {
                // We know its a call, so lets treat it like a call
                Call * call = static_cast<Call*>(ast);

                validate_call(*call);

                // TOOD: 

                // >>>> Iterate over call params. call 'check_value_is_valid_for_assignment' on the ValType of the parameter
                //      to ensure we promote the expression if needed, and stop compiling if something is incorrect

                // WRITE THE ENDECODED ENCODE CALL 

                error_man.report_custom("Analyzer", " >>>>> CALL NOT DONE", true);

                // Encode the call to something we can handle in the intermediate layer
                return endecoder.encode(call);
            }

            case NodeType::VAL : 
            { 
                // Check that the raw value is one that is valid within the current assignment
                check_value_is_valid_for_assignment(ast->val_type, c, et, id);
                return ast->value;
            }
            
            //  This is where we convert NodeType to the Assignment type. This should make it so we can change the actual tokens in the language
            //  without having to modify this statement

            case NodeType::ADD    :return (validate_assignment_ast(ast->l, c, et, id) + " " + validate_assignment_ast(ast->r, c, et, id) + " ADD    " );  
            case NodeType::SUB    :return (validate_assignment_ast(ast->l, c, et, id) + " " + validate_assignment_ast(ast->r, c, et, id) + " SUB    " );
            case NodeType::DIV    :return (validate_assignment_ast(ast->l, c, et, id) + " " + validate_assignment_ast(ast->r, c, et, id) + " DIV    " );
            case NodeType::MUL    :return (validate_assignment_ast(ast->l, c, et, id) + " " + validate_assignment_ast(ast->r, c, et, id) + " MUL    " );
            case NodeType::MOD    :return (validate_assignment_ast(ast->l, c, et, id) + " " + validate_assignment_ast(ast->r, c, et, id) + " MOD    " );
            case NodeType::POW    :return (validate_assignment_ast(ast->l, c, et, id) + " " + validate_assignment_ast(ast->r, c, et, id) + " POW    " );
            case NodeType::LTE    :return (validate_assignment_ast(ast->l, c, et, id) + " " + validate_assignment_ast(ast->r, c, et, id) + " LTE    " );
            case NodeType::GTE    :return (validate_assignment_ast(ast->l, c, et, id) + " " + validate_assignment_ast(ast->r, c, et, id) + " GTE    " );
            case NodeType::GT     :return (validate_assignment_ast(ast->l, c, et, id) + " " + validate_assignment_ast(ast->r, c, et, id) + " GT     " );
            case NodeType::LT     :return (validate_assignment_ast(ast->l, c, et, id) + " " + validate_assignment_ast(ast->r, c, et, id) + " LT     " );
            case NodeType::EQ     :return (validate_assignment_ast(ast->l, c, et, id) + " " + validate_assignment_ast(ast->r, c, et, id) + " EQ     " );
            case NodeType::NE     :return (validate_assignment_ast(ast->l, c, et, id) + " " + validate_assignment_ast(ast->r, c, et, id) + " NE     " );
            case NodeType::LSH    :return (validate_assignment_ast(ast->l, c, et, id) + " " + validate_assignment_ast(ast->r, c, et, id) + " LSH    " );
            case NodeType::RSH    :return (validate_assignment_ast(ast->l, c, et, id) + " " + validate_assignment_ast(ast->r, c, et, id) + " RSH    " );
            case NodeType::BW_OR  :return (validate_assignment_ast(ast->l, c, et, id) + " " + validate_assignment_ast(ast->r, c, et, id) + " BW_OR  " );
            case NodeType::BW_XOR :return (validate_assignment_ast(ast->l, c, et, id) + " " + validate_assignment_ast(ast->r, c, et, id) + " BW_XOR " );
            case NodeType::BW_AND :return (validate_assignment_ast(ast->l, c, et, id) + " " + validate_assignment_ast(ast->r, c, et, id) + " BW_AND " );
            case NodeType::OR     :return (validate_assignment_ast(ast->l, c, et, id) + " " + validate_assignment_ast(ast->r, c, et, id) + " OR     " );
            case NodeType::AND    :return (validate_assignment_ast(ast->l, c, et, id) + " " + validate_assignment_ast(ast->r, c, et, id) + " AND    " );
            case NodeType::BW_NOT :return (validate_assignment_ast(ast->l, c, et, id) + " BW_NOT ");
            case NodeType::NEGATE :return (validate_assignment_ast(ast->l, c, et, id) + " NEGATE "  );
            case NodeType::RETURN :return (validate_assignment_ast(ast->l, c, et, id) + " RETURN "  );
            case NodeType::ROOT   : error_man.report_custom("Analyzer", "ROOT NODE found in arithmetic exp", true); break;
            default:
            return "Its dead, jim";
        }
        return "Complete";
    }
}