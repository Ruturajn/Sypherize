# High Level Design of a Compiler

Following are some steps that one can take to design and implement
a basic compiler.

## Approach

### LEXER

- Define the syntax for the language you want to compile. This would include
  stuff like, variable declarations, assignments, function definitions,
  return types, etc. Decide, what your delimiters, and expression terminators
  will be.
- The delimiters can be a whitespace, carriage return, newline character,
  colons, commas, parenthesis, etc.
- Start writing the compiler in **C** (Obviously !!).
- Begin, by getting the path to the file, that needs to be compiled as an
  argument. This would generally happen in the main function.
- With this path, open the file, and read its contents into a string.
- Define error types and do error handling.
- Now, this string needs to be parsed and *lexed* (which is a fancy term
  for tokenizing).

### PARSER and AST

- ***This part is going to be tricky***. Now, that the tokens are created 
  they need to be conceptualized.
- A list of identifiers that have been seen previously needs to be
  maintained, so that we can deal with something like:
  ```
  int a;
  a = 0;
  ```
  and, not allow something like:
  ```
  a = 0;
  ```
  where `a` has been defined without assigning a *type* to it.
- Support *Scoped identifier definitions*, which will allow
  for something like this:
  ```
  int a = 0;

  def_func foo(int a, int b) -> int {
        // Do something with `a` here,
        // which refers to the `a` passed
        // to this function.
  }
  ```
  Now, to do this, an *AST* (**A**bstract **S**yntax **T**ree) is required.
  For this, there needs to be way to represent *expressions* as control
  flow diagrams, which can be constructed with binary trees.
  Possible expressions can be:
    - `if` statements, with a condition, if-body and an else-body.
    - Loops, both `for` and `while`, which have a terminating condition, an
      upper bound and a increment factor in the case of a `for` loop.
    - Variable assignments.
    - Statements with the ternary operator.
    - And many more.

## References

- https://www.youtube.com/playlist?list=PLysa8wRFCssxGKj_RxBWr3rwmjEYlJIpa
