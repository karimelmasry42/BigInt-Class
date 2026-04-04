# BigInt Class

`BigInt` is an arbitrary-precision signed integer class for C++. It is designed for working with whole numbers that exceed the range of built-in integer types such as `int`, `long`, and `long long`.

The class stores values as digit strings and performs arithmetic manually, digit by digit. This makes it suitable for large-number calculations where fixed-width integer types overflow.

## Overview

`BigInt` supports:

- arbitrarily large positive and negative integers
- construction from integers and strings
- arithmetic with operator overloading
- relational comparisons
- stream input and output
- increment and decrement operations

Examples of valid values:

- `0`
- `42`
- `-99`
- `123456789012345678901234567890`

## Features

### Construction

The class can be created from:

- a default zero value
- a signed 64-bit integer
- a string representation of an integer
- another `BigInt` through copy construction

### Arithmetic Operators

The class supports full integer arithmetic:

- addition: `+`, `+=`
- subtraction: `-`, `-=`
- multiplication: `*`, `*=`
- division: `/`, `/=`
- modulus: `%`, `%=`

Division is integer division and truncates toward zero.

### Unary and Update Operators

The class supports:

- unary plus: `+x`
- unary minus: `-x`
- pre-increment: `++x`
- post-increment: `x++`
- pre-decrement: `--x`
- post-decrement: `x--`

### Comparison Operators

The following comparisons are supported:

- `==`
- `!=`
- `<`
- `<=`
- `>`
- `>=`

### Stream Support

The class can be used directly with C++ streams:

- output with `cout << value`
- input with `cin >> value`

## Number Representation Rules

`BigInt` follows these normalization rules:

- leading zeros are removed
- zero is always stored as non-negative
- negative zero is never produced

Examples:

- `"007"` becomes `"7"`
- `"-0"` becomes `"0"`

## Arithmetic Behavior

### Sign Handling

The class handles signed arithmetic for all operator combinations:

- adding numbers with the same sign preserves that sign
- adding numbers with different signs subtracts magnitudes and uses the sign of the larger magnitude
- multiplication and division use the standard XOR sign rule
- modulus keeps the sign of the dividend

Examples:

- `BigInt(-5) * BigInt(3)` -> `-15`
- `BigInt(-10) / BigInt(3)` -> `-3`
- `BigInt(-10) % BigInt(3)` -> `-1`
- `BigInt(10) % BigInt(-3)` -> `1`

### Error Handling

Division and modulus by zero throw:

```cpp
std::runtime_error("Division by zero")
```

## Typical Use Cases

`BigInt` is useful for:

- very large integer calculations
- educational implementations of arbitrary-precision arithmetic
- operator overloading practice in C++
- problems involving values beyond native integer limits
- manual integer arithmetic without external libraries

## Example

```cpp
#include <iostream>

int main() {
    BigInt a("12345678901234567890");
    BigInt b("98765432109876543210");

    std::cout << "a + b = " << a + b << '\n';
    std::cout << "a * b = " << a * b << '\n';
    std::cout << "-a = " << -a << '\n';
    std::cout << "a < b = " << (a < b) << '\n';
}
```

## Build

Compile with a C++17-compatible compiler:

```bash
g++ -std=c++17 -Wall -Wextra *.cpp -o bigint
```

## Summary

`BigInt` provides a clean operator-based interface for arbitrary-size signed integer arithmetic in C++. It combines large-number support with familiar syntax, making it practical for both experimentation and general integer-heavy programming tasks.
