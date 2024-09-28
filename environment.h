#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

#include <cassert>
#include <map>
#include <string>
#include "value.h"

class Environment {
private:
  Environment *m_parent;
  //representation of environment (map of names to values)
  std::map<std::string, Value> variables;

  // copy constructor and assignment operator prohibited
  Environment(const Environment &);
  Environment &operator=(const Environment &);

public:
  Environment(Environment *parent = nullptr);
  ~Environment();

  void define(const std::string& name, const Value& value);
  Value lookup(const std::string& name) const;
  void assign(const std::string& name, const Value& value);
  void assign_local(const std::string& name, const Value& value);
  bool is_defined(const std::string &name) const {
        if (variables.find(name) != variables.end()) {
            return true;
        }
        return m_parent ? m_parent->is_defined(name) : false;
    }

};

#endif // ENVIRONMENT_H
