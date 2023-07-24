# High-Level Design of a Compiler

Following are some steps that one can take to design and implement
a basic compiler.

## Approach

### LEXER

- Define the syntax for the language you want to compile. This would include
  stuff like variable declarations, assignments, function definitions,
  return types, etc. Decide your delimiters, and expression terminator.
- The delimiters are generally whitespace, carriage returns (if you are on
  Windows) and newline characters, but they can be anything based on your
  choice and taste.
- Start writing the compiler in **C** (Obviously !!).
- Begin, by storing the data from the file that needs to be compiled into
  a buffer.
- Now, this buffer needs to be *lexed* (which is a fancy term
  for tokenizing) and parsed.

### PARSER and AST

- ***This part is going to be tricky***. Now, that the tokens are created 
  they need to be conceptualized.
- A list of identifiers that have been seen previously needs to be
  maintained so that we can deal with something like:
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
  To do this, an *AST* (**A**bstract **S**yntax **T**ree) is required.
  For this, there needs to be a way to represent *expressions* as control
  flow diagrams, which can be constructed with binary trees.
  Possible expressions can be:
    - `if` statements, with a condition, if-body, and an else-body.
    - Loops, both `for` and `while`, which have a terminating condition, an
      upper bound, and an increment factor in the case of a `for` loop.
    - Variable assignments.
    - Statements with the ternary operator.
    - And many more.
- For the AST we need to define some data structures that will store
  important bits and pieces of information from our source code, which
  will finally help during code generation.
- First and foremost we will need a node that will act as the basic
  building block of the AST. It will store the *type* of the node,
  its *value*, and a pointer to its child and next child, which gives
  us a tree-like structure.
- There needs to be a binding between variables and their
  types, which will enable us to do type-checking and proper code generation.
  This will be achieved by having a linked list that stores these bindings
  between variables in that particular context, which brings us to the next
  point.
- A parsing *context* will be required for us to have *scopes* as described earlier.
  This context structure will store a parent context (which will allow for
  parsing function bodies, and then returning back to where we left off),
  an *operator* node (this will be explained at a later stage), and pointers
  to three types of environments. These will be the *type* environment,
  which stores the binding between a type and its size in bytes, the *variables*
  environment which stores the binding between a variable and its type, and
  finally, the *functions* environment that stores the bindings between
  function names and their bodies.
- With the concept of an AST, parsing contexts, environments, and bindings,
  we can move ahead and go along with constructing the AST and setting up
  the environments, and bindings on the way.
- Let's consider the following source code:
  ```
  int: a := 340;
  ```
  The variable `a` is **declared** and **initialized** in the first line, and then
  it's **reassigned** to a new value. For the construction of an AST, we need to
  familiarize ourselves with the *grammar* of the source code that we will be compiling.
  For declaring `a`, firstly its *type* is declared, which happens to be an `int` in this
  case, then there is the name of the variable separated by a `:` suggesting that `a` is
  of *type* `int`. This completes our variable declaration. The code then further goes
  on to initialize the value of `a` and assign an `int` to it using the `:=` operator,
  which then completes our variable initialization. This statement as a whole
  forms an **expression**, and any source code can be represented as a list of expressions
  that can be used to construct an AST.
- So, our goal here is to decipher the kind of expression we are parsing and place it
  into the AST, with all the information we will need about it, to generate
  code for that particular expression.
- Now, let's look at the first expression in the example code given above. *What are the
  important pieces of information we will need?*
    - Firstly the **type** of the expression. An expression can be anything a variable
      declaration, a variable re-assignment (second expression in the code), a function
      definition, an expression inside a function, a function call, an if-else statement,
      a loop, etc. So we need to understand the type of expression and store it. Hence,
      we need an AST node of type ***variable declaration***.
    - Next, we will need to store the name of the variable, which is known as an
      identifier or a **symbol**. So, we will need a node of type *symbol*.
    - Finally, we need the value that it is initialized to. For this, we will need
      a node that denotes the types of value that it is storing, and since it's
      an `int` in this case, we will require a node of type **int**.
  
  With all of this information, we can form a simple tree that represents a variable
  declaration.
  ```
  VAR_DECL
         |-- SYM: a
          `- INT: 340 
  ```
  Congrats! We have generated a logical representation of our source code. Although, we
  aren't done yet. _Remember the environments, bindings, and contexts we talked about earlier?_
  Yes, we need to set them up. So, as of now let's not concern ourselves with nested contexts
  and stuff like that, just to keep it simple and get a basic understanding of things. So,
  for now, we just have the global context which has the variable declaration for `a`. The
  context will be initialized, and the types environment will contain the bindings between
  the types (the symbol that represents the type, e.g. `int` itself is a string that can be
  considered as a symbol) that our language supports and their sizes in bytes.
  
  We are yet to actually store the *type* of `a`, which will be done by binding `a` to the
  type `int` in the variables environment within a global context
  (Storing these bindings will help once we encounter variable re-assignments).
  With that, we have now successfully parsed a variable declaration.
- Similarly, we can parse variable *re-assignments*, when we encounter a pattern
  in the source code that looks like a variable re-assignment. We can create a new node
  of type *variable re-assignment* and add that into the AST.
  ```
  a := 10;
  ```
  When we come across such an expression in the source code, we need to make sure that
  the variable was declared and/or initialized somewhere before in the code. This is where
  our environments come into the picture. The variable declarations store bindings
  in the environments, for us to validate in the future that a variable re-assignment
  is only allowed when we can successfully look up that variable in the environment. It
  also helps us check that the assignment to the variable is of a valid type. Apart from
  that, we can make sure that a variable re-definition doesn't take place when we already have
  a binding present for that variable in the environment. So, when a variable re-assignment
   is encountered we add a new node representing it into the AST.
  ```
  VAR_RE_ASSIGN
               |-- SYM: a
                `- INT: 10 
  ```
- Now that we have parsed two expressions, we also need to link them together somehow to
  create a flow in the program. So, we create a new node of type *program*, and add the
  representations of these expressions to that node as children, which results in this.
  ```
  PROGRAM
         |-- VAR_DECL
         |          |-- SYM: a
         |           `- INT: 340
         |
          `- VAR_RE_ASSIGN
                         |-- SYM: a
                          `- INT: 10
  ```
- We now know how to create an AST for two types of expressions, next let's take a look
  at parsing function definitions and their bodies.
- The source for functions in *Sypher* looks something like this.
  ```
  def_func sample_func(int: a, int: x) ~ int {
    x := 33;
  }
  ```
  It doesn't really do much, but that's not the point here. The goal is to parse our
  function and represent it logically in our AST. Let's try to understand the grammar
  first. The use of the *keyword* `def_func` helps us define a function. After this
  we expect the name of the function to appear, which is followed by a list of
  arguments that the function accepts, along with their types within parentheses.
  Finally, we have the `~` sign, which denotes that the following is the return
  type of the function. Then within braces, exists the function body, which can
  be represented as a list of expressions in a nested scope or *context*.
- For creating the AST for our function:
    - We will need a node of type *function*.
    - Then we will need to store the list of parameters it accepts and
      their types.
    - After that, we will require the return type of the function.
    - Finally, we will have the function body, which can be considered
      a list of expressions.

  With all of this information, we can add the representation for
  our function to the AST.
  ```
    PROGRAM
          |-- VAR_DECL
          |          |-- SYM: a
          |           `- INT: 340
          |
          |-- VAR_RE_ASSIGN
          |                |-- SYM: a
          |                 `- INT: 10
          |
           ` FUNCTION
                     |-- PARAMETER_LIST
                     |                |-- PARAM
                     |                |        |-- SYM: a
                     |                |         `- SYM: int
                     |                |  
                     |                 `-- PARAM
                     |                         |-- SYM: x
                     |                          `- SYM: int
                     |
                     |-- SYM: int (Return type)
                      `- LIST_OF_EXPRESSIONS
                                            `- VAR_RE_ASSIGN
                                                            |-- SYM: x
                                                             `- INT: 33
  ```
  Great, we have now parsed our function into the AST. Then, we need
  to setup a child context, and store our variable bindings into
  that environment, to allow for nested scopes. So, e.g. `x` will
  be bound in the context within the *variables* environment
  with a type `int` in the function's scope. Finally, we also bind
  the function name with the actual function node in the AST within
  the *functions* environment.

### Type Checker

### Code Generation - Assembly (x86_64)

## References

- https://www.youtube.com/playlist?list=PLysa8wRFCssxGKj_RxBWr3rwmjEYlJIpa
