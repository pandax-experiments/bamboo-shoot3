# BSI

Bamboo Shoot Interface files are sources from which code for serializable
types are generated.  This documents its syntax.

## Format

The whole file is a list of declarations, each can be an enum declaration
or struct declaration.

- enum declaration:
```cpp
enum NAME : UNDERLYING_TYPE {
  ENUMERATOR[=VALUE] [, ENUMERATOR[=VALUE]...]
};
```
This is compiled to enum classes in C++.

- struct declaration:
```cpp
struct NAME {
  TYPE MEMBER_NAME = TAG;
  ...
};
```
where `TYPE` is either a type name, or a `[TYPE]` which denotes a list, or
a `(KEYTYPE => VALUETYPE)` which denotes a map.  In C++, lists become
`std::vector` and maps become `std::map`.  Builtin types are `string`,
`int8`, `int16`, `int32`, `int64`, `uint8`, `uint16`, `uint32`, `uint64`,
`float` and `double`.

- extern declaration:
```cpp
extern NAME;
```
Declares `NAME` as a type.  Compiles to nothing; the compiler just assumes
such a type exists.  Useful to include types defined in C++ as members of
tuples or structs.

## Using bsic

Compiler for BSI files is `bsic`, which will be placed at top of the build
tree when you run `make bsic`.  Its usage is:

```
Usage: bsic [option...] input...
  where option is:
  -o <filename>  --output=<filename>            output file
  -g <filename>  --random-generator=<filename>  random generators file
  -t <filename>  --tests=<filename>             tests file
```

All inputs are processed as if they were concatenated into one file, and
the outputs contain inputs from all files.

Output file is the generated C++ header of declarations; formats of random
generators file and tests file are internal and may change at any time.

## Notes

Namespaces are not supported, include guards are not defined, STL headers
are not included.  To use the generated header you need to wrap around it,
define include guards, include required STL headers and optionally wrap in
a namespace.  Required headers are:

```cpp
#include <cstdint>
#include <string>
#include <vector>
#include <map>
```
