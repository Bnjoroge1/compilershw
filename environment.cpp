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

// TODO: implement member functions
void Environment::define(const std::string& name, const Value& value) {
    
    variables[name] = value;

}

Value Environment::lookup(const std::string& name) const {
    //std::cout << "Looking up variable " << name << " in environment " << this << std::endl;

    auto it = variables.find(name);
    if (it != variables.end()) {
       //std::cout << "Found variable '" << name << "' with value " << it->second.as_str() << " in environment " << this << std::endl;
        if (m_parent != nullptr) {
            // If we're not in the global scope, try to find in parent environments
            try {
                return m_parent->lookup(name);
            } catch (const EvaluationError&) {
                // If not found in parent, use the local value
                return it->second;
            }
        }
        return it->second;
    }

    if (m_parent != nullptr) {
        //std::cout << "Not found, looking in parent environment " << m_parent << std::endl;
        return m_parent->lookup(name);
    }

    //std::cout << "Variable '" << name << "' not found in any scope" << std::endl;
    EvaluationError::raise(Location(), "Variable '%s' not defined", name.c_str());
}
void Environment::assign_local(const std::string& name, const Value& value) {
    variables[name] = value;
}
void Environment::assign(const std::string& name, const Value& value) {
    auto it = variables.find(name);
    if (it != variables.end()) {
        // If the variable exists in this scope, update it
        //std::cout << "Variable found in current scope, updating" << std::endl;
        it->second = value;
    } else if (m_parent != nullptr) {
        // If not found in this scope and there's a parent, try to assign in the parent
        //std::cout << "Variable not found, trying parent environment " << m_parent << std::endl;
        m_parent->assign(name, value);
        //EvaluationError::raise(Location(), "Variable '%s' not defined", name.c_str());
    } else {
        // If we're in the global scope and the variable doesn't exist, create it
        //variables[name] = value;
               // std::cout << "Variable not found in any scope, raising error" << std::endl;

        EvaluationError::raise(Location(), "Variable '%s' not defined", name.c_str());
    }
}
