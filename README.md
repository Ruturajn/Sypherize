# Sypherc

**Sypherc** (with an *S*) is a compiler written in C, for a C-like language
called **Sypher**. Sypher is aimed to be a general-purpose statically typed
programming language. The [`examples`](https://github.com/Ruturajn/Sypherize/tree/main/examples)
directory contains some sample programs that demonstrate all the features that the
language currently supports. See [`Usage`](https://github.com/Ruturajn/Sypherize/tree/main#usage) for more details.

> **Note**
> 
> This project is currently under development, and a few fundamental features are supported as of now.
> As the project matures, there will be better documentation detailing the language and all the related
> intricacies.

## Overwiew

This project is my take on writing a compiler by following [`Lens_r's`](https://www.youtube.com/playlist?list=PLysa8wRFCssxGKj_RxBWr3rwmjEYlJIpa)
playlist on youtube which outlines the complete process
of designing and implementing a compiler.


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

- Using `gcc`:
```
$ gcc code_gen.s -o code_gen
```

- Using GNU binutils:
    - Assemble into an object file.
      ```
      $ as code_gen.S -o code_gen.o
      ```
    - Link into the final executable.
      ```
      $ ld code_gen.o -o code_gen
      ```

## Usage

```
Sypherc 0.1
Ruturajn <nanotiruturaj@gmail.com>
Compiler for Sypher

USAGE:
    sypherc [OPTIONS] INPUT_FILE

OPTIONS:
    -f, --format <OUTPUT_FORMAT>
            A valid output format for code generation
            VALID FORMATS:
            - `x86_64-windows`
            - `default`

    -h, --help
            Print this help information

    -i, --input <INPUT_FILE_PATH>
            Path to the input file

    -o, --output <OUTPUT_FILE_PATH>
            Path to the output file

    -v, --version
            Print out current version of Sypherize

    -V, --verbose
            Print out extra debugging information

NOTE - Everything else is treated as an input file
```

## Miscellaneous

The file [`ROAD_MAP.md`](https://github.com/Ruturajn/Sypherize/blob/main/ROAD_MAP.md)
is a summary of what I picked up from the playlist, which includes a set of
steps, that describe the whole design. It's mainly for my understanding, but
you can definitely give it a read if you like.
