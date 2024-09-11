#include "environment.h"
#include "exceptions.h"
#include "location.h"

Environment::Environment(Environment *parent)
  : m_parent(parent) {
  assert(m_parent != this);
}

Environment::~Environment() {
}

// TODO: implement member functions
void Environment::define(const std::string& name, const Value& value) {
    // Check if the variable is already defined
    if (variables.find(name) != variables.end()) {
        // You might want to throw an exception here, or just silently overwrite
        // For now, we'll just overwrite
    }
    variables[name] = value;
}

Value Environment::lookup(const std::string& name) const {
    auto it = variables.find(name);
    if (it == variables.end()) {
        // Variable not found, throw an exception
        EvaluationError::raise(Location(), "Variable '%s' not defined", name.c_str());
    }
    return it->second;
}

void Environment::assign(const std::string& name, const Value& value) {
    auto it = variables.find(name);
    if (it == variables.end()) {
        // Variable not found, throw an exception
        EvaluationError::raise(Location(), "Cannot assign to undefined variable '%s'", name.c_str());
    }
    it->second = value;
}

