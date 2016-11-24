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
|-------------------------------------|---------------------------------------------------------|-----------------|
| primary                             | primary                                                 | (same)          |
| enum                                | enum                                                    | (same)          |
| pbss::var_uint                      | variable length unsigned integer                        | (same)          |
| std::tuple, std::pair               | heterogeneous static-length sequence of members         | (same)          |
| C array [T]                         | homogeneous dynamic-length sequence of T                | (not supported) |
| std::array<T>                       | homogeneous dynamic-length sequence of T                | (same)          |
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

## Classes

### Buffer

```cpp
class uninitialized_byte {
  uninitialized_byte();
  uninitialized_byte(char);
  operator char() const;
};

using buffer = std::vector<uninitialized_byte>;
```

`uninitialized_byte` is a tiny wrapper class for storing uninitialized
chars in `std::vector`; being very simple, it is best to read its
[source](../include/bs3/pbss/uninitialized-byte.hh) for usage.  And
`buffer` is that.  Currently no extra APIs are added on top of `vector`;
you may want to do `reinterpret_cast` on element addresses.

### Stream-like classes

```cpp
template <class Char, class Traits=std::char_traits<Char> >
struct basic_char_range_reader {
  basic_char_range_reader(const Char* first, const Char* last);
  basic_char_range_reader& read(Char* dest, std::streamsize count);
  bool eof() const;
  typename Traits::int_type get();
  typename Traits::int_type peek();
  void ignore(std::streamsize count);
};

using char_range_reader = basic_char_range_reader<char>;
```

`std::istream` like class for reading from an array `const Char`, in the
range [first, last).

```cpp
template <class Char, class Traits=std::char_traits<Char> >
struct basic_char_range_writer {
  basic_char_range_writer(Char* dest);
  basic_char_range_writer& put(Char ch);
  basic_char_range_writer& write(const Char* src, std::streamsize count);
};

using char_range_writer = basic_char_range_writer<char>;
```

`std::ostream` like class for writing to a `char*`.  There is no check on
overflow.

For all of these classes, refer to `std::istream` or `std::ostream` for
semantics of provided functions.

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
`write` for single-byte writes, and will be preferred for such writes.

```cpp
template <class T>
std::string serialize_to_string(const T&);

template <class T>
buffer serialize_to_buffer(const T&);
```

Helpers for serializing directly in memory.  `serialize_to_buffer` is
currently the fastest implementation.  It is worth noting that these helper
functions, when inlined into a whole, usually produce shorter and faster
code; but default inline thresholds in typical compilers are usually too
low for a moderately complex structure.  You may inspect the binary to
check, and if that is a problem, try tweaking your compiler options,
e.g. in GCC by `-finline-limit=N` or in Clang by `-mllvm
-inline-threshold=N`; to limit the effective scope of such tweaks, tweak
with a dedicated translation unit that only does explicit instantiation.

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

```cpp
template <class T>
T parse_from_string(const std::string&);

template <class T>
T parse_from_buffer(const buffer&);
```

Helpers for parsing from memory.

For large custom structs (that is, with many members), it tries to do
optimistic parsing, assuming the input is generated from the same schema.
If the input does not match, it falls back to normal parsing, which ensures
compatibility but is significantly slower.  The "many members" threshold
can be tweaked by defining a macro `PBSS_STRUCT_OPTIMISTIC_PARSE_THRESHOLD`
before including pbss headers; the default value is 8.

## Errors

### early_eof_error
`parse` may throw `early_eof_error` if given stream ends before a full
value can be parsed:
```cpp
class early_eof_error : public std::runtime_error;
```
