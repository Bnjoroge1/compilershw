#include "exceptions.h"
#include "ast.h"

ASTTreePrint::ASTTreePrint() {
}

ASTTreePrint::~ASTTreePrint() {
}

std::string ASTTreePrint::node_tag_to_string(int tag) const {
  switch (tag) {
  case AST_ADD:
    return "ADD";
  case AST_SUB:
    return "SUB";
  case AST_MULTIPLY:
    return "MULTIPLY";
  case AST_DIVIDE:
    return "DIVIDE";
  case AST_VARREF:
    return "VARREF";
  case AST_INT_LITERAL:
    return "INT_LITERAL";
  case AST_UNIT:
    return "UNIT";
  case AST_STATEMENT:
    return "STATEMENT";
  case AST_VAR_DECLARATION:
    return "VARDEF";
  case AST_ASSIGN:
    return "ASSIGN";
  case AST_LOGICAL_AND:
    return "LOGICAL_AND";
  case AST_LOGICAL_OR:
    return "LOGICAL_OR";
  case AST_LESS:
    return "LT";
  case AST_LESS_EQUAL:
    return "LTE";
  case AST_GREATER:
    return "GT";
  case AST_GREATER_EQUAL:
    return "GTE";
  case AST_EQUAL:
    return "EQ";
  case AST_NOT_EQUAL:
    return "NEQ";
  case AST_IF:
    return "IF";
  case AST_WHILE:
    return "WHILE";
  case AST_FUNCTION:
    return "FUNCTION";
  case AST_PARAMETER_LIST:
    return "PARAMETER_LIST";
  case AST_STATEMENT_LIST:
    return "STATEMENT_LIST";
  case AST_FNCALL:
    return "FNCALL";
  case AST_ARGLIST:
    return "ARGLIST";
  default:
    return "UNKNOWN_NODE_TYPE_" + std::to_string(tag);
  }
}
