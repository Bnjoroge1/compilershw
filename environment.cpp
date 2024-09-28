#include "environment.h"
#include "exceptions.h"
#include "location.h"
#include <iostream>

Environment::Environment(Environment *parent)
  : m_parent(parent) {
  assert(m_parent != this);
}

Environment::~Environment() {
}

void Environment::define(const std::string& name, const Value& value) {
    variables[name] = value;

}

Value Environment::lookup(const std::string& name) const {
    

    auto it = variables.find(name);
    if (it != variables.end()) {
        return it->second;
    }
    else if (m_parent != nullptr) {
        return m_parent->lookup(name);
    }
    else {
        EvaluationError::raise(Location(), "Variable '%s' not defined", name.c_str());
    }
  
}
void Environment::assign_local(const std::string& name, const Value& value) {
    variables[name] = value;
}
void Environment::assign(const std::string& name, const Value& value) {
    auto it = variables.find(name);
    if (it != variables.end()) {
        // If the variable exists in this scope, update it
        
        it->second = value;
    } else if (m_parent != nullptr) {
        // If not found in this scope and there's a parent, try to assign in the parent
        
        m_parent->assign(name, value);
        
    } else {
        // If the variable is not found in any scope, raise an error
        EvaluationError::raise(Location(), "Variable '%s' not defined", name.c_str());
    }
}
