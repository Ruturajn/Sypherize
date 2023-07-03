# Sypherize

**Sypherize** (with an *S*) is a compiler written in C, for a C like language
called **Sypher**.

## Overwiew

I will be developing this project by following
[`Lens_r's`](https://www.youtube.com/playlist?list=PLysa8wRFCssxGKj_RxBWr3rwmjEYlJIpa)
playslist on youtube which outlines the complete process
of desigining and implementing a compiler.

The file [`RoadMap.md`](https://github.com/Ruturajn/Sypherize/blob/main/RoadMap.md)
is a summary of what I picked up from the playslist, which include a set of
steps, that outline the whole design.

## Build Instructions

Dependencies:
- `GCC`
- `Make`

Optional Dependencies for building docs:
- `Doxygen`
- `Latex`

Compiling this project:

- Clone the repository.
  ```
  $ git clone https://github.com/Ruturajn/Sypherize.git
  ```
- Navigate to the `Sypherize` directory and call make.
  ```
  $ make
  ```
- Use `$ make help` to look at available targets.

## Compiling the Generated x86_64 Assembly File

Using `gcc`:
```
$ gcc code_gen.S -o code_gen
```
<!--
- Use an assembler and linker:
    - Assemble into an object file.
      ```
      $ as code_gen.S -o code_gen.o
      ```
    - Link into the final executable.
      ```
      $ ld code_gen.o -subsystem-console -o code_gen
      ```
-->
