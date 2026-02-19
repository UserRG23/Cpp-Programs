### Overview 

This repository contains my solutions for the BIE-PA2 course. While PA1 was about the basics, this course dives deep into C++, focusing on the object-oriented paradigm, abstract data types, and efficient resource management.

Key areas of focus:

- **OOP Principles**: Encapsulation, Inheritance, and Polymorphism.
- **Memory Management**: Moving beyond manual malloc to RAII, smart pointers, and deep vs. shallow copying.
- **Standard Template Library (STL)**: Effective use of containers, iterators, and algorithms.
- **Generic Programming**: Writing reusable code using C++ templates.

### Repository structure

Each directory represents a specific task. You will find:
- **Task Documentation**: A `README.md` within each directory explaining the specific problem
- **Build System**: A `Makefile` in every directory to streamline compilation and testing
- **Source Code**: Implementation files following course standards.

### Getting Started 

To compile and run the tasks, ensure you have `gdb`, `g++` and `make` installed. Use the following commands from within a task directory:

| Command      | Action |
| ------------ | ------ |
| `make build` | Compiles the code and places the binary in the `bin/` directory. |
| `make run`   | Compiles the code and executes it immediately. |
| `make debug` | Compiles the code and launches `gdb` for debugging. |
