# PBSS

This documents public interface for PandaX Bamboo Shoot Serialization,
PBSS, implementing the serialization format for C++.

## Entry

The whole library can be included by
```cpp
#include <bs3/pbss/pbss.hh>
```
and all functions are defined in `namespace pbss`.

## Supported types

The list of supported C++ types, and how they are serialized/parsed is:

| Type                                | Serialized to                                           | Parses from     |
|-------------------------------------+---------------------------------------------------------+-----------------|
| primary                             | primary                                                 | (same)          |
| enum                                | enum                                                    | (same)          |
| pbss::var_uint                      | variable length unsigned integer                        | (same)          |
| std::pair<L, R>                     | heterogeneous static-length sequence of L, R            | (same)          |
| C array [T]                         | homogeneous dynamic-length sequence of T                | (not supported) |
| std::array<T>                       | homogeneous dynamic-length sequence of T                | (not supported) |
| STL dynamic containers              | homogeneous dynamic-length sequence of its `value_type` | (same)          |
| Custom struct with tags (see below) | heterogeneous tagged sequence                           | (same)          |
| Custom struct as tuple (see below)  | heterogeneous static-length sequence of members         | (same)          |

STL dynamic containers require either `push_back` or `insert` to be
available.

### Tagging custom structs

Tagged structs automatically gets serializing and parsing.  Tagging is
performed by two macros, `PBSS_TAGGED_STRUCT(PBSS_TAG_MEMBER(id, memptr),
...)`, like this:
```cpp
struct simple {
  int16_t a;
  double b;

  PBSS_TAGGED_STRUCT(
    PBSS_TAG_MEMBER(1, &simple::a),
    PBSS_TAG_MEMBER(2, &simple::b));
};
```

If any tagged member is not `public` or is cv-qualified (i.e. `const`
and/or `volatile`), the behavior is undefined.

Parsing requires the type to be value-initializable, and missing fields
from the input are left untouched and remains the value set by
value-initialization.

### Custom struct as tuple

Tuple structs are not automatically forward/backward-compatible, but is
faster and more compact by eliminating extra metadata.  A tuple struct can
be marked like:
```cpp

struct simple {
  char a;
  int b;

  PBSS_TUPLE_MEMBERS(
    PBSS_TUPLE_MEMBER(&simple::a),
    PBSS_TUPLE_MEMBER(&simple::b));
};
```

If any tagged member is not `public` or is cv-qualified (i.e. `const`
and/or `volatile`), the behavior is undefined.

Parsing requires the type to be value-initializable, and missing fields
from the input are left untouched and remains the value set by
value-initialization.

## Functions

### Serializing

```cpp
template </* unspecified */>
void serialize(Stream& stream, const T& object);
```

Serialize `object` to `stream`.  The template parameter may or may not be
`T`, so it is not to be relied on; just use auto-deduced template type as
in `serialize(stream, object)`.

`Stream` must implement these methods:
```cpp
class Stream {
public:
  /*unused*/ put(char);
  /*unused*/ write(const char* src, std::streamsize count);
};
```

Refer to `std::ostream` for semantics.  `put` is expected to be faster than
`write` for single-bytes writes, and will be preferred for such writes.

There is a `char_range_writer` implementing this interface, which is the
most optimized implementation, and a helper function `serialize_to_string`
using this writer.

### Parsing

```cpp
template <class T, class Stream>
T parse(Stream& stream);
```

Parse an object of type `T` from `stream`.  The template parameter decides
the return type, so call it as `parse<T>(stream)`.  If `stream` ends before
a full value can be parsed, an instance of
[`early_eof_error`](#early_eof_error) is thrown.

`Stream` must implement these methods:
```cpp
class Stream {
  /*unused*/ read(char* dest, std::streamsize count);
  bool eof() /*optionally const*/;
  std::char_traits<char>::int_type get();
  std::char_traits<char>::int_type peek();
  /*unused*/ ignore(std::streamsize count);
};
```

Refer to `std::istream` for semantics.  `get` is expected to be faster than
`read` for single-byte reads, and will be preferred for such reads.

There is a `char_range_reader` implementing this interface, which is the
most optimized implementation, and a helper function `parse_from_string`
using this reader.

## Errors

### early_eof_error
`parse` may throw `early_eof_error` if given stream ends before a full
value can be parsed:
```cpp
class early_eof_error : public std::runtime_error;
```
