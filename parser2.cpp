#include <cassert>
#include <map>
#include <string>
#include <memory>
#include "token.h"
#include "ast.h"
#include "exceptions.h"
#include "parser2.h"
#include <iostream>


////////////////////////////////////////////////////////////////////////
// Parser2 implementation
// This version of the parser builds an AST directly,
// rather than first building a parse tree.
////////////////////////////////////////////////////////////////////////

// This is the grammar (Unit is the start symbol):
//
// Unit -> Stmt
// Unit -> Stmt Unit
// Stmt -> E ;
// E -> T E'
// E' -> + T E'
// E' -> - T E'
// E' -> epsilon
// T -> F T'
// T' -> * F T'
// T' -> / F T'
// T' -> epsilon
// F -> number
// F -> ident
// F -> ( E )

Parser2::Parser2(Lexer *lexer_to_adopt)
  : m_lexer(lexer_to_adopt)
  , m_next(nullptr) {
}

Parser2::~Parser2() {
  delete m_lexer;
}

Node *Parser2::parse() {
  return parse_Unit();
}
Node *Parser2::parse_Unit() {
  // note that this function produces a "flattened" representation
  // of the unit

  std::unique_ptr<Node> unit(new Node(AST_UNIT));
  for (;;) {
    unit->append_kid(parse_TStmt());
    if (m_lexer->peek() == nullptr)
      break;
  }

  return unit.release();
}

Node *Parser2::parse_Stmt() {
    Node *next = m_lexer->peek();
    if (next == nullptr) {
        EvaluationError::raise(m_lexer->get_current_loc(), "Unexpected end of input");
    }
    //parsing the if statement
    if (next->get_tag() == TOK_IF) {
        return parse_If_Stmt();
    } else if (next->get_tag() == TOK_WHILE) {
      //parsing the while statement
        return parse_While_Stmt();
    } else if (next->get_tag() == TOK_VAR) {
        return parse_var_declaration();
    } else {
        Node *stmt = new Node(AST_STATEMENT);
        stmt->append_kid(parse_A());
        expect(TOK_SEMICOLON);
        return stmt;
    }
}
Node *Parser2::parse_TStmt() {
  // A top-level statement can be a regular statement or a function definition
  Node *next = m_lexer->peek();
  if (next == nullptr) {
    return nullptr;
  }

  if (next->get_tag() == TOK_FUNCTION) {
    return parse_Func();
  } else {
    return parse_Stmt();
  }
}

Node *Parser2::parse_If_Stmt() {

    Node *if_stmt = new Node(AST_IF);
    
    expect(TOK_IF);
    expect(TOK_LPAREN);
    Node *condition = parse_A();
    if (condition == nullptr) {
        delete if_stmt;
        EvaluationError::raise(m_lexer->peek()->get_loc(), "Failed to parse condition in if statement");
    }
    if_stmt->append_kid(condition);
    expect(TOK_RPAREN);
    expect(TOK_LBRACE);
    Node *if_body = parse_SList();
    if (if_body == nullptr) {
        delete if_stmt;
        EvaluationError::raise(m_lexer->peek()->get_loc(), "Failed to parse body of if statement");
    }

    expect(TOK_RBRACE);
    
    if_stmt->append_kid(if_body);
    
    Node *next = m_lexer->peek();
    if (next != nullptr && next->get_tag() == TOK_ELSE) {
        expect(TOK_ELSE);
        expect(TOK_LBRACE);
        Node *else_body = parse_SList();
        if (else_body == nullptr) {
            delete if_stmt;
            EvaluationError::raise(m_lexer->peek()->get_loc(), "Failed to parse body of else statement");
        }
        expect(TOK_RBRACE);
        if_stmt->append_kid(else_body);
    }
    
    return if_stmt;
}
Node *Parser2::parse_While_Stmt() {
    expect(TOK_WHILE);
    expect(TOK_LPAREN);
    Node *condition = parse_A();
    expect(TOK_RPAREN);
    expect(TOK_LBRACE);
    Node *body = parse_SList();
    expect(TOK_RBRACE);
    
    Node *while_stmt = new Node(AST_WHILE);
    while_stmt->append_kid(condition);
    while_stmt->append_kid(body);
    
    return while_stmt;
}
Node *Parser2::parse_Func() {
    Node *func_def = new Node(AST_FUNCTION);
    
    expect(TOK_FUNCTION);
    Node *func_name = expect(TOK_IDENTIFIER);
    // a VARREF node for the function name
    Node *func_name_ref = new Node(AST_VARREF);
    func_name_ref->set_str(func_name->get_str());
    func_def->append_kid(func_name_ref);
    
    expect(TOK_LPAREN);
    Node *params = parse_OptPList();
    func_def->append_kid(params);
    expect(TOK_RPAREN);
    expect(TOK_LBRACE);
    Node *body = parse_SList();
    func_def->append_kid(body);
    expect(TOK_RBRACE);
    
    return func_def;
}


Node *Parser2::parse_SList() {
    Node *stmt_list = new Node(AST_STATEMENT_LIST);
    while (m_lexer->peek()->get_tag() != TOK_RBRACE) {
        stmt_list->append_kid(parse_Stmt());
    }
    return stmt_list;
}




Node *Parser2::parse_A() {
    Node *first_tok = m_lexer->peek();
    Node *second_tok = m_lexer->peek(2);

    if (first_tok->get_tag() == TOK_IDENTIFIER && second_tok->get_tag() == TOK_ASSIGN) {
        Node *assign = new Node(AST_ASSIGN);
        Node *lhs = new Node(AST_VARREF);
        lhs->set_str(expect(TOK_IDENTIFIER)->get_str());
        expect(TOK_ASSIGN);
        Node *rhs = parse_A();
        assign->append_kid(lhs);
        assign->append_kid(rhs);
        return assign;
    } else {
        return parse_L();
    }
}

Node *Parser2::parse_L() {
    Node *l = parse_R();
    
    Node *next_tok = m_lexer->peek();
    if (next_tok != nullptr) {
        if (next_tok->get_tag() == TOK_LOGICAL_OR) {
            m_lexer->next(); // consume '||'
            Node *right = parse_R();
            l = new Node(AST_LOGICAL_OR, {l, right});
        } else if (next_tok->get_tag() == TOK_LOGICAL_AND) {
            m_lexer->next(); // consume '&&'
            Node *right = parse_R();
            l = new Node(AST_LOGICAL_AND, {l, right});
        }
    }
    
    return l;
}

Node *Parser2::parse_R() {
  Node *e = parse_E(); // Start with parse_E() to handle all expression types, including addition
  while (true) {
    Node *next_tok = m_lexer->peek();
    if (next_tok == nullptr) break;
    if (next_tok->get_tag() == TOK_LESS || next_tok->get_tag() == TOK_LESS_EQUAL ||
        next_tok->get_tag() == TOK_GREATER || next_tok->get_tag() == TOK_GREATER_EQUAL ||
        next_tok->get_tag() == TOK_EQUAL || next_tok->get_tag() == TOK_NOT_EQUAL) {
      int op = next_tok->get_tag();
      m_lexer->next(); // consume the operator
      Node *right = parse_E(); 
      int ast_tag;
      switch (op) {
        case TOK_LESS: ast_tag = AST_LESS; break;
        case TOK_LESS_EQUAL: ast_tag = AST_LESS_EQUAL; break;
        case TOK_GREATER: ast_tag = AST_GREATER; break;
        case TOK_GREATER_EQUAL: ast_tag = AST_GREATER_EQUAL; break;
        case TOK_EQUAL: ast_tag = AST_EQUAL; break;
        case TOK_NOT_EQUAL: ast_tag = AST_NOT_EQUAL; break;
        default: assert(false);
      }
      e = new Node(ast_tag, {e, right});
    } else {
      break;
    }
  }
  return e;
}
Node *Parser2::parse_E() {
  Node *t = parse_T();
  Node *result = parse_EPrime(t);
  return result;
}




// This function is passed the "current" portion of the AST
// that has been built so far for the additive expression.
Node *Parser2::parse_EPrime(Node *ast_) {
  // E' -> ^ + T E'
  // E' -> ^ - T E'
  // E' -> ^ epsilon

  std::unique_ptr<Node> ast(ast_);

  // peek at next token
  Node *next_tok = m_lexer->peek();
  if (next_tok != nullptr) {
    int next_tok_tag = next_tok->get_tag();
    if (next_tok_tag == TOK_PLUS || next_tok_tag == TOK_MINUS)  {
      // E' -> ^ + T E'
      // E' -> ^ - T E'
      std::unique_ptr<Node> op(expect(static_cast<enum TokenKind>(next_tok_tag)));

      // build AST for next term, incorporate into current AST
      Node *term_ast = parse_T();
      ast.reset(new Node(next_tok_tag == TOK_PLUS ? AST_ADD : AST_SUB, {ast.release(), term_ast}));

      // copy source information from operator node
      ast->set_loc(op->get_loc());

      // continue recursively
      return parse_EPrime(ast.release());
    }
  }

  // E' -> ^ epsilon
  // No more additive operators, so just return the completed AST
  return ast.release();
}

Node *Parser2::parse_T() {
  // T -> F T'

  // Parse primary expression
  Node *ast = parse_F();

  // Recursively continue the multiplicative expression
  return parse_TPrime(ast);
}

Node *Parser2::parse_TPrime(Node *ast_) {
  // T' -> ^ * F T'
  // T' -> ^ / F T'
  // T' -> ^ epsilon

  std::unique_ptr<Node> ast(ast_);

  // peek at next token
  Node *next_tok = m_lexer->peek();
  if (next_tok != nullptr) {
    int next_tok_tag = next_tok->get_tag();
    if (next_tok_tag == TOK_TIMES || next_tok_tag == TOK_DIVIDE)  {
      // T' -> ^ * F T'
      // T' -> ^ / F T'
      std::unique_ptr<Node> op(expect(static_cast<enum TokenKind>(next_tok_tag)));

      // build AST for next primary expression, incorporate into current AST
      Node *primary_ast = parse_F();
      ast.reset(new Node(next_tok_tag == TOK_TIMES ? AST_MULTIPLY : AST_DIVIDE, {ast.release(), primary_ast}));

      // copy source information from operator node
      ast->set_loc(op->get_loc());

      // continue recursively
      return parse_TPrime(ast.release());
    }
  }

  // T' -> ^ epsilon
  // No more multiplicative operators, so just return the completed AST
  return ast.release();
}

Node *Parser2::parse_F() {
  Node *tok = m_lexer->peek();

  if (tok->get_tag() == TOK_INTEGER_LITERAL) {
    m_lexer->next();  
    Node *node = new Node(AST_INT_LITERAL);
    node->set_str(tok->get_str());
    return node;
  } else if (tok->get_tag() == TOK_IDENTIFIER) {
    m_lexer->next();  
    Node *node = new Node(AST_VARREF);
    node->set_str(tok->get_str());
    
    // Check if this is a function call
    if (m_lexer->peek()->get_tag() == TOK_LPAREN) {
      Node *func_call = new Node(AST_FNCALL);
      func_call->append_kid(node);  // Use the VARREF as the function name
      expect(TOK_LPAREN);
      func_call->append_kid(parse_OptArgList());
      expect(TOK_RPAREN);
      return func_call;
    }
    
    return node;
  } else if (tok->get_tag() == TOK_LPAREN) {
    m_lexer->next();  
    Node *node = parse_A();
    expect(TOK_RPAREN);
    return node;
  } else {
    error_at_current_loc("Unexpected token in factor");
    return nullptr;
  }
}
Node *Parser2::parse_OptPList() {
    Node *param_list = new Node(AST_PARAMETER_LIST);
    if (m_lexer->peek()->get_tag() != TOK_RPAREN) {
        param_list = parse_PList();
    }
    // If it's an empty parameter list (epsilon production),
    // we've already created an empty AST_PARAMETER_LIST node
    return param_list;
}

Node *Parser2::parse_PList() {
    Node *param_list = new Node(AST_PARAMETER_LIST);
    do {
        Node *param = expect(TOK_IDENTIFIER);
        param_list->append_kid(new Node(AST_VARREF, param->get_str()));
        if (m_lexer->peek()->get_tag() != TOK_COMMA) break;
        expect(TOK_COMMA);
    } while (true);
    return param_list;
}

Node *Parser2::parse_OptArgList() {
    Node *arg_list = new Node(AST_ARGLIST);
    if (m_lexer->peek()->get_tag() != TOK_RPAREN) {
        arg_list = parse_ArgList();
    }
    
    return arg_list;
}

Node *Parser2::parse_ArgList() {
    Node *arg_list = new Node(AST_ARGLIST);
    do {
        arg_list->append_kid(parse_L());
        if (m_lexer->peek()->get_tag() != TOK_COMMA) break;
        expect(TOK_COMMA);
    } while (true);
    return arg_list;
}

Node *Parser2::expect(enum TokenKind tok_kind) {
  std::unique_ptr<Node> next_terminal(m_lexer->next());
  if (next_terminal->get_tag() != tok_kind) {
    SyntaxError::raise(next_terminal->get_loc(), "Unexpected token '%s'", next_terminal->get_str().c_str());
  }
  return next_terminal.release();
}

void Parser2::expect_and_discard(enum TokenKind tok_kind) {
  Node *tok = expect(tok_kind);
  delete tok;
}

void Parser2::error_at_current_loc(const std::string &msg) {
  SyntaxError::raise(m_lexer->get_current_loc(), "%s", msg.c_str());
}
//parses the var declaration and returns the AST
Node *Parser2::parse_var_declaration() {
    Node *stmt = new Node(AST_STATEMENT);
    Node *vardef = new Node(AST_VAR_DECLARATION);
    expect(TOK_VAR);
    Node *varref = new Node(AST_VARREF);
    varref->set_str(expect(TOK_IDENTIFIER)->get_str());
    vardef->append_kid(varref);
    stmt->append_kid(vardef);
    expect(TOK_SEMICOLON);
    return stmt;
}