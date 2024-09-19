## Assignment 1 Description

I started by extending the lexical analyzer to recognize new tokens such as 'var', '=', '||', '&&', and comparison operators. For the parser, I implemented new grammar productions to handle variable declarations, assignments, logical operations, and comparisons. I used a recursive descent parsing approach, with two tokens of lookahead for handling assignments correctly. I created new AST node types to represent these new language constructs. In the interpreter, I implemented the analyze() function to perform semantic analysis, primarily checking that variables are declared before use. The execute() function evaluates the AST, handling variable declarations, assignments, and expression evaluation. I used a recursive approach for expression evaluation, with each AST node type having its own evaluation logic.

One interesting technique I used was implementing short-circuit evaluation for logical AND and OR operations, which improved efficiency by avoiding unnecessary computations. I also used std::unique_ptr for managing dynamically allocated AST nodes to prevent memory leaks. A challenging aspect was handling operator precedence correctly, especially with the new logical and comparison operators. I solved this by carefully structuring the grammar productions and the corresponding parsing functions. 

I had a challenge correctly formatting the output for the error test cases to pass the location information.
Instead of this output: input/error05.in:2:5: Error: Reference to undefined name 'b'
I had this output:Error: input/<unknown>:-1:-1: Error: Reference to undefined name 'b'
