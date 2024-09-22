#ifndef INTERP_H
#define INTERP_H

#include "value.h"
#include "environment.h"
#include <set>
#include <string>


class Node;
class Location;

class Interpreter {
private:
  Node *m_ast;
  std::vector<Function*> m_functions;
 

public:
  Interpreter(Node *ast_to_adopt);
  ~Interpreter();

  void analyze();
  Value execute();
  static Value intrinsic_print(Value args[], unsigned num_args,
                               const Location &loc, Interpreter *interp);
  static Value intrinsic_println(Value args[], unsigned num_args,
                                     const Location &loc, Interpreter *interp);
  static Value intrinsic_readint(Value args[], unsigned num_args, const Location &loc, Interpreter *interp);




private:
  // TODO: private member functions
  void analyze_node(Node *node, std::set<std::string>& defined_vars);
  Value evaluate_node(Node *node, Environment &env);
};

#endif // INTERP_H
