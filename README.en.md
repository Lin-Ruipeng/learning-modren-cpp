# Modern C++ Study

A learning project focused on the core features of smart pointers in C++11/14/17.

## Project Overview

This project demonstrates the usage of modern C++ smart pointers through practical examples to help understand memory management mechanisms.

## Directory Structure

```
├── Class01_unique_ptr/          # Exclusive ownership smart pointer
│   ├── 1.cpp                    # Basic usage of unique_ptr
│   ├── 2.cpp                    # Practical applications of unique_ptr
│   └── note.md                  # Learning notes
│
├── Class02_shared_ptr/          # Shared ownership smart pointer
│   ├── 1.cpp                    # Basic usage of shared_ptr
│   ├── 2.cpp                    # Advanced features of shared_ptr
│   └── note.md                  # Learning notes
│
└── prompt.md                    # Teaching framework
```

## Learning Content

### Class01: unique_ptr - Exclusive Ownership

- `make_unique<T>()` to create an exclusive pointer
- `.reset()` to reset the pointer
- `.release()` to release ownership
- `std::move()` for move semantics

### Class02: shared_ptr - Shared Ownership

- `make_shared<T>()` to create a shared pointer
- Copy construction and assignment
- Custom deleters
- `.use_count()` to check reference count

## Compilation and Execution

```bash
# Compile unique_ptr examples
g++ -std=c++14 Class01_unique_ptr/1.cpp -o unique_ptr_demo1
g++ -std=c++14 Class01_unique_ptr/2.cpp -o unique_ptr_demo2

# Compile shared_ptr examples
g++ -std=c++14 Class02_shared_ptr/1.cpp -o shared_ptr_demo1
g++ -std=c++14 Class02_shared_ptr/2.cpp -o shared_ptr_demo2
```

## Dependencies

- C++14 or higher standard
- Compiler supporting C++11 smart pointers

## License

MIT License