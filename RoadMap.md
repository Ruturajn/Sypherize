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

- *This part is going to be tricky*. Now, that the tokens are created 
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

## References

- https://www.youtube.com/playlist?list=PLysa8wRFCssxGKj_RxBWr3rwmjEYlJIpa
