#include <cassert>
#include <algorithm>
#include <memory>
#include "ast.h"
#include "node.h"
#include "exceptions.h"
#include "function.h"
#include "interp.h"
#include <set>
#include <string>
#include <iostream>
#include <limits>



Interpreter::Interpreter(Node *ast_to_adopt)
  : m_ast(ast_to_adopt) {
    
}

Interpreter::~Interpreter() {
    for (Function *func: m_functions) {
        delete func;
        
    }
   //delete m_ast;
}
static const std::set<std::string> intrinsic_functions = {"print", "println", "readint"};

void Interpreter::analyze() {
  std::set<std::string> defined_vars;
  analyze_node(m_ast, defined_vars);
}
void Interpreter::analyze_node(Node *node, std::set<std::string>& defined_vars) {
    if (node == nullptr) {
        return;
    }


    switch (node->get_tag()) {
          case AST_FUNCTION: {
            // Add the function name to defined_vars
            Node* func_name_node = node->get_kid(0);
            if (func_name_node != nullptr) {
                defined_vars.insert(func_name_node->get_str());
            }
            
            // Create a new set for the function's scope
            std::set<std::string> function_vars = defined_vars;
            
            // Add parameters to the function's scope
            Node* params = node->get_kid(1);
            for (unsigned i = 0; i < params->get_num_kids(); ++i) {
                std::string param_name = params->get_kid(i)->get_str();
                if (function_vars.find(param_name) != function_vars.end()) {
                    const Location& loc = params->get_kid(i)->get_loc();
                    std::string error_msg = "input/" + loc.get_srcfile() + ":" +
                                            std::to_string(loc.get_line()) + ":" +
                                            std::to_string(loc.get_col()) +
                                            ": Error: Parameter '" + param_name + "' is already defined";
                    SemanticError::raise(loc, error_msg.c_str());
                }
                function_vars.insert(param_name);
            }
            
            // Analyze the function body with the new scope
            analyze_node(node->get_kid(2), function_vars);
            break;
        }
        case AST_VARREF: {
            std::string var_name = node->get_str();
            if (defined_vars.find(var_name) == defined_vars.end() && intrinsic_functions.find(var_name) == intrinsic_functions.end()) {
                
                const Location& loc = node->get_loc();
                std::string error_msg = "input/" + loc.get_srcfile() + ":" +
                                        std::to_string(loc.get_line()) + ":" +
                                        std::to_string(loc.get_col()) +
                                        ": Error: Reference to undefined name '" + var_name + "'";
                SemanticError::raise(loc, error_msg.c_str());
            }
        }
            break;
    
        case AST_VAR_DECLARATION:
            if (node->get_num_kids() > 0) {
                Node* var_name_node = node->get_kid(0);
                if (var_name_node != nullptr) {
                    defined_vars.insert(var_name_node->get_str());
                } 
            } 
            break;
        case AST_INT_LITERAL:
            // Do nothing for integer literals
            break;
        case AST_ADD:
            // Fall through to default case for child analysis
        default:
            // Recursively analyze child nodes
            for (unsigned i = 0; i < node->get_num_kids(); ++i) {
                try {
                    Node* child = node->get_kid(i);
                    if (child != nullptr) {
                        analyze_node(child, defined_vars);
                    } else {
                    }
                } catch (const std::out_of_range& e) {
                    break;
                }
            }
            break;
    }
}

Value Interpreter::execute() {
    if (m_ast == nullptr) {
        EvaluationError::raise(Location(), "Empty AST");
    }

    Environment env;
    Value result;

    // Bind intrinsic functions to the global environment
    env.define("print", Value(&Interpreter::intrinsic_print));
    env.define("println", Value(&Interpreter::intrinsic_println));
    env.define("readint", Value(&Interpreter::intrinsic_readint));



    // Check if m_ast has any children
    if (m_ast->get_num_kids() == 0) {
        // AST is empty (no statements), return a default value
        return Value(0);
    }

    // Iterate through all statements in the AST
    for (unsigned i = 0; i < m_ast->get_num_kids(); ++i) {
        Node *stmt = m_ast->get_kid(i);
        if (stmt == nullptr) continue;

        switch (stmt->get_tag()) {
            case AST_VAR_DECLARATION: {
                if (stmt->get_num_kids() > 0) {
                    std::string var_name = stmt->get_kid(0)->get_str();
                    env.define(var_name, Value(0));
                    result = Value(0);
                } else {
                    EvaluationError::raise(stmt->get_loc(), "Invalid variable declaration");
                }
                break;
            }
            case AST_STATEMENT: {
                if (stmt->get_num_kids() > 0) {
                    result = evaluate_node(stmt->get_kid(0), env);
                } else {
                    EvaluationError::raise(stmt->get_loc(), "Empty statement");
                }
                break;
            }
            case AST_IF:
            case AST_ASSIGN:
            case AST_STATEMENT_LIST:
            case AST_WHILE:
            case AST_FUNCTION:
                result = evaluate_node(stmt, env);
                break;
            default:
                EvaluationError::raise(stmt->get_loc(), "Unexpected statement type");
        }
    }

    return result;
}

Value Interpreter::evaluate_node(Node *node, Environment &env) {
    if (node == nullptr) {
        
        EvaluationError::raise(Location(), "Null node encountered in evaluate_node");
    }
    

    
    switch (node->get_tag()) {
        case AST_UNIT:
            {
                Value last_value(0);
                for (unsigned i = 0; i < node->get_num_kids(); ++i) {
                    last_value = evaluate_node(node->get_kid(i), env);
                }
                return last_value;
            }
        case AST_STATEMENT:
            if (node->get_num_kids() > 0) {
                return evaluate_node(node->get_kid(0), env);
            }
            return Value(0); // Empty statement
        case AST_STATEMENT_LIST: {
            Environment block_env(&env);
            
            Value result;
            for (unsigned i = 0; i < node->get_num_kids(); ++i) {
                Node *stmt = node->get_kid(i);
                result = evaluate_node(stmt, block_env);
            }
            return result;  
    }
        case AST_INT_LITERAL:
            return Value(std::stoi(node->get_str()));
        case AST_VAR_DECLARATION:
    if (node->get_num_kids() > 0) {
        Node* var_name_node = node->get_kid(0);
        if (var_name_node != nullptr && var_name_node->get_tag() == AST_VARREF) {
            std::string var_name = var_name_node->get_str();
            env.define(var_name, Value(0));
            return Value(0);
        }
    }
    return Value(0);  // Variable declarations evaluate to 0
    
        case AST_VARREF:
            return env.lookup(node->get_str());
        case AST_ASSIGN: {
            std::string var_name = node->get_kid(0)->get_str();
            Value value = evaluate_node(node->get_kid(1), env);
            env.assign(var_name, value);
            return value;
        }
        case AST_ADD: {
            Value left = evaluate_node(node->get_kid(0), env);
            Value right = evaluate_node(node->get_kid(1), env);
            if (!left.is_numeric() || !right.is_numeric()) {
                EvaluationError::raise(node->get_loc(), "Operands of '+' must be numeric");
            }
                    return Value(left.get_ival() + right.get_ival());
        }
        case AST_SUB:
            {
            Value left = evaluate_node(node->get_kid(0), env);
            Value right = evaluate_node(node->get_kid(1), env);
            if (!left.is_numeric() || !right.is_numeric()) {
                EvaluationError::raise(node->get_loc(), "Operands of '-' must be numeric");
            }
                    return Value(left.get_ival() - right.get_ival());
        }
        case AST_MULTIPLY:
        {
            Value left = evaluate_node(node->get_kid(0), env);
            Value right = evaluate_node(node->get_kid(1), env);
            if (!left.is_numeric() || !right.is_numeric()) {
                EvaluationError::raise(node->get_loc(), "Operands of '*' must be numeric");
            }
            return Value(left.get_ival() * right.get_ival());
        }   
        case AST_DIVIDE: {
            int divisor = evaluate_node(node->get_kid(1), env).get_ival();
            if (divisor == 0) {
                EvaluationError::raise(node->get_loc(), "Division by zero");
            }
            return Value(evaluate_node(node->get_kid(0), env).get_ival() / divisor);
        }
        case AST_LOGICAL_AND: {
            Value left = evaluate_node(node->get_kid(0), env);
            if (left.get_ival() == 0) return Value(0); // Short-circuit evaluation
            Value right = evaluate_node(node->get_kid(1), env);
            return Value((left.get_ival() != 0 && right.get_ival() != 0) ? 1 : 0);
        }
        
        case AST_LESS: {
            Value left = evaluate_node(node->get_kid(0), env);
            Value right = evaluate_node(node->get_kid(1), env);
            if (!left.is_numeric() || !right.is_numeric()) {
                EvaluationError::raise(node->get_loc(), "Operands of '<' must be numeric");
            }
            return Value(left.get_ival() < right.get_ival());
        }
        
        case AST_GREATER_EQUAL: {
            Value left = evaluate_node(node->get_kid(0), env);
            Value right = evaluate_node(node->get_kid(1), env);
            return Value(left.get_ival() >= right.get_ival() ? 1 : 0);
        }
        case AST_LOGICAL_OR:
            return Value((evaluate_node(node->get_kid(0), env).get_ival() != 0 ||
                          evaluate_node(node->get_kid(1), env).get_ival() != 0) ? 1 : 0);

        case AST_GREATER:{
            Value left = evaluate_node(node->get_kid(0), env);
            Value right = evaluate_node(node->get_kid(1), env);
            if (!left.is_numeric() || !right.is_numeric()) {
                EvaluationError::raise(node->get_loc(), "Operands of '<' must be numeric");
            }
            return Value(left.get_ival() > right.get_ival());
        }
        
        
        case AST_LESS_EQUAL:
            return Value(evaluate_node(node->get_kid(0), env).get_ival() <= 
                         evaluate_node(node->get_kid(1), env).get_ival() ? 1 : 0);
        case AST_EQUAL:
            return Value(evaluate_node(node->get_kid(0), env).get_ival() == 
                         evaluate_node(node->get_kid(1), env).get_ival() ? 1 : 0);
        case AST_NOT_EQUAL:
            return Value(evaluate_node(node->get_kid(0), env).get_ival() != 
                         evaluate_node(node->get_kid(1), env).get_ival() ? 1 : 0);
        case AST_IF: {
            Value condition = evaluate_node(node->get_kid(0), env);
            if (!condition.is_numeric()) {
                EvaluationError::raise(node->get_loc(), "Condition in if statement is not an integer");
            }
            if (condition.get_ival() != 0) {
                //evaluate the 'if' body
                Environment if_env(&env);

                evaluate_node(node->get_kid(1), env);
        } else if(node->get_num_kids()> 2){
                //evaluate the else body block.

                Environment else_env(&env);

                evaluate_node(node->get_kid(2), env);
            }
            return Value(0);   //return 0
        }
        case AST_WHILE: {
        while (true) {
            Value condition = evaluate_node(node->get_kid(0), env);
            if (!condition.is_numeric()) {
                EvaluationError::raise(node->get_loc(), "Condition in while statement is not numeric");
            }
            if (condition.get_ival() == 0) {
                break;  // Exit the loop if the condition is false
            }
            Environment while_env(&env);
            // Execute the body of the while loop
            evaluate_node(node->get_kid(1), while_env);
            }
            return Value(0);  // While statements always evaluate to 0
        }
        case AST_FUNCTION: {
            std::string func_name = node->get_kid(0)->get_str();
            std::vector<std::string> param_names;
            Node *params = node->get_kid(1);
            for (unsigned i = 0; i < params->get_num_kids(); ++i) {
                    param_names.push_back(params->get_kid(i)->get_str());
                }
            Node *body = node->get_kid(2);
            Function *func = new Function(func_name, param_names, &env, body);
            m_functions.push_back(func);  
            env.define(func_name, Value(func));

            return Value(0);  // Function definitions evaluate to 0
        }
       case AST_FNCALL: {
    std::string func_name = node->get_kid(0)->get_str();
    Value func_val = env.lookup(func_name);

    // Evaluate arguments
    Node *arg_list = node->get_kid(1);
    std::vector<Value> args;
    for (unsigned i = 0; i < arg_list->get_num_kids(); ++i) {
        args.push_back(evaluate_node(arg_list->get_kid(i), env));
    }

    if (func_val.get_kind() == VALUE_FUNCTION) {
        Function *func = func_val.get_function();

        // Check number of arguments
        if (args.size() != func->get_num_params()) {
            EvaluationError::raise(node->get_loc(), 
                "Incorrect number of arguments for function '%s': expected %u, got %zu", 
                func_name.c_str(), func->get_num_params(), args.size());
        }
               

        // Create new environment for function call
        Environment call_env(func->get_parent_env());

        // Bind arguments to parameters
        for (unsigned i = 0; i < args.size(); ++i) {
            call_env.define(func->get_params()[i], args[i]);
        }

        // Evaluate function body
        Node* body = func->get_body();
        Value result;
        for (unsigned i = 0; i < body->get_num_kids(); ++i) {
            Node* stmt = body->get_kid(i);
            if (stmt->get_tag() == AST_ASSIGN) {
                std::string var_name = stmt->get_kid(0)->get_str();
                Value value = evaluate_node(stmt->get_kid(1), call_env);
                if (!call_env.is_defined(var_name)) {
                    call_env.define(var_name, value);
                } else {
                    call_env.assign(var_name, value);
                }
            } else {
                result = evaluate_node(stmt, call_env);
                // If this is the last statement, it's implicitly the return value
                if (i == body->get_num_kids() - 1) {
                    return result;
                }
            }
        }

return result;  // In case there are no statements in the function body
    } else if (func_val.get_kind() == VALUE_INTRINSIC_FN) {
        // Handle intrinsic functions
        IntrinsicFn fn = func_val.get_intrinsic_fn();
        return fn(&args[0], args.size(), node->get_loc(), this);
    } else if (func_val.get_kind() == VALUE_INTRINSIC_FN) {
        IntrinsicFn fn = func_val.get_intrinsic_fn();
        return fn(args.data(), args.size(), node->get_loc(), this);
    } else {
        EvaluationError::raise(node->get_loc(), 
            "Called value '%s' is not a function", func_name.c_str());
    }
}
        default:
            
            EvaluationError::raise(node->get_loc(), 
                ("Unknown node type in expression: " + std::to_string(node->get_tag())).c_str());
    }
    return Value(0); // This line should never be reached
}

Value Interpreter::intrinsic_print(Value args[], unsigned num_args,
                                   const Location &loc, Interpreter *interp) {
  if (num_args != 1) {
        EvaluationError::raise(loc, "Wrong number of arguments passed to print function");
    }
    printf("%s", args[0].as_str().c_str());
    fflush(stdout);  // Ensure output is immediately visible
    return Value(0);  
}
Value Interpreter::intrinsic_println(Value args[], unsigned num_args,
                                     const Location &loc, Interpreter *interp) {
  if (num_args != 1) {
    EvaluationError::raise(loc, "Wrong number of arguments passed to println function");
  }
  printf("%s\n", args[0].as_str().c_str());
  fflush(stdout);  // Ensure output is immediately visible
  return Value(0);  
}

Value Interpreter::intrinsic_readint(Value args[], unsigned num_args, const Location &loc, Interpreter *interp) {
    if (num_args != 0) {
        EvaluationError::raise(loc, "readint function does not accept any arguments");
    }
    
    int input;
    while (!(std::cin >> input)) {
        std::cin.clear(); // clear error flags
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // discard invalid input
    }
    
    return Value(input);
}