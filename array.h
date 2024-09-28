#ifndef ARRAY_H
#define ARRAY_H

#include "valrep.h"
#include "value.h"
#include <vector>
#include <string>

class Array : public ValRep {
private:
    std::vector<Value> m_elements;

public:
    Array();
    virtual ~Array();

    // Array operations
    void append(const Value& val);
    Value get(int index) const;
    void set(int index, const Value& val);
    size_t size() const;
    Value pop();

    // Conversion to string
    std::string to_string() const;

    // Override from ValRep
    Array* as_array() override;
};

#endif // ARRAY_H