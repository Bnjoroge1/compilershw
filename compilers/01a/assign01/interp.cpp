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


Interpreter::Interpreter(Node *ast_to_adopt)
  : m_ast(ast_to_adopt) {
}

Interpreter::~Interpreter() {
  delete m_ast;
}

void Interpreter::analyze() {
  std::set<std::string> defined_vars;
  analyze_node(m_ast, defined_vars);
}
void Interpreter::analyze_node(Node *node, std::set<std::string>& defined_vars) {
    if (node == nullptr) {
        return;
    }


    switch (node->get_tag()) {
        case AST_VARREF: {
            std::string var_name = node->get_str();
            if (defined_vars.find(var_name) == defined_vars.end()) {
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
                } else {
                }
            } else {
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
            default:
                EvaluationError::raise(stmt->get_loc(), "Unexpected statement type");
        }
    }

    return result;
}

Value Interpreter::evaluate_node(Node *node, Environment &env) {
    switch (node->get_tag()) {
        case AST_INT_LITERAL:
            return Value(std::stoi(node->get_str()));
        case AST_VARREF:
            return env.lookup(node->get_str());
        case AST_ASSIGN: {
            std::string var_name = node->get_kid(0)->get_str();
            Value value = evaluate_node(node->get_kid(1), env);
            env.assign(var_name, value);
            return value;
        }
        case AST_ADD:
            return Value(evaluate_node(node->get_kid(0), env).get_ival() + 
                         evaluate_node(node->get_kid(1), env).get_ival());
        case AST_SUB:
            return Value(evaluate_node(node->get_kid(0), env).get_ival() - 
                         evaluate_node(node->get_kid(1), env).get_ival());
        case AST_MULTIPLY:
            return Value(evaluate_node(node->get_kid(0), env).get_ival() * 
                         evaluate_node(node->get_kid(1), env).get_ival());
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
            return Value(left.get_ival() < right.get_ival() ? 1 : 0);
        }
        
        case AST_GREATER_EQUAL: {
            Value left = evaluate_node(node->get_kid(0), env);
            Value right = evaluate_node(node->get_kid(1), env);
            return Value(left.get_ival() >= right.get_ival() ? 1 : 0);
        }
        case AST_LOGICAL_OR:
            return Value((evaluate_node(node->get_kid(0), env).get_ival() != 0 ||
                          evaluate_node(node->get_kid(1), env).get_ival() != 0) ? 1 : 0);
        case AST_GREATER:
            return Value(evaluate_node(node->get_kid(0), env).get_ival() > 
                         evaluate_node(node->get_kid(1), env).get_ival() ? 1 : 0);
        
        
        case AST_LESS_EQUAL:
            return Value(evaluate_node(node->get_kid(0), env).get_ival() <= 
                         evaluate_node(node->get_kid(1), env).get_ival() ? 1 : 0);
        case AST_EQUAL:
            return Value(evaluate_node(node->get_kid(0), env).get_ival() == 
                         evaluate_node(node->get_kid(1), env).get_ival() ? 1 : 0);
        case AST_NOT_EQUAL:
            return Value(evaluate_node(node->get_kid(0), env).get_ival() != 
                         evaluate_node(node->get_kid(1), env).get_ival() ? 1 : 0);
        default:
            EvaluationError::raise(node->get_loc(), "Unknown node type in expression");
    }
    return Value(0); // This line should never be reached
}