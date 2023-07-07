# Sypherize

**Sypherize** (with an *S*) is a compiler written in C, for a C like language
called **Sypher**.

## Overwiew

I will be developing this project by following
[`Lens_r's`](https://www.youtube.com/playlist?list=PLysa8wRFCssxGKj_RxBWr3rwmjEYlJIpa)
playlist on youtube which outlines the complete process
of designing and implementing a compiler.

The file [`ROAD_MAP.md`](https://github.com/Ruturajn/Sypherize/blob/main/ROAD_MAP.md)
is a summary of what I picked up from the playlist, which includes a set of
steps, that describe the whole design. It's mainly for my understanding, but
you can definitely give it a read if you like.


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
