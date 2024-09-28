#include "array.h"
#include "exceptions.h"
#include <string>
#include <vector>


Array::Array()
    : ValRep(VALREP_ARRAY) {}

Array::~Array() {}

void Array::append(const Value& val) {
     //append the value to the end of the array
    m_elements.push_back(val);
}

Value Array::get(int index) const {
     //get the value at the index
    if (index < 0 || index >= static_cast<int>(m_elements.size())) {
        EvaluationError::raise(Location(), "Array index out of bounds");
    }
    return m_elements[index];
}

void Array::set(int index, const Value& val) {
     //set the value at the index
    if (index < 0 || index >= static_cast<int>(m_elements.size())) {
        EvaluationError::raise(Location(), "Array index out of bounds");
    }
    m_elements[index] = val;
}

size_t Array::size() const {
     //get the size of the array
    return m_elements.size();
}

Value Array::pop() {
     //pop the last element from the array
    if (m_elements.empty()) {
        EvaluationError::raise(Location(), "Cannot pop from an empty array");
    }
    Value last = m_elements.back();
    m_elements.pop_back();
    return last;
}

std::string Array::to_string() const {
     //convert the array to a string representation
    std::string result = "[";
    for (size_t i = 0; i < m_elements.size(); ++i) {
        if (i > 0) result += ", ";
        result += m_elements[i].as_str();
    }
    result += "]";
    return result;
}

Array* Array::as_array() {
    return this;
}