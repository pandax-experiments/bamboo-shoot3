# Bamboo Shoot 3 object serialization

Design goal of the serialization is to be efficient, reasonably compact,
and zero external dependency.

Type support shall include
- primary types,
- custom structs,
- and STL containers;
with features
- static typing,
- forward/backward compatibility for custom structs,
- and NO support for cross referencing.

## Format

Supported types, and their serialized format, are listed below:

- Primary types, as defined by
  [`std::is_arithmetic`](http://en.cppreference.com/w/cpp/types/is_arithmetic),
  are serialized to their little endian representation, occupying exactly
  `sizeof(type)` bytes.

- Enums, are serialized the same as their underlying type.

- A homogeneous dynamic-length sequence, in which all elements are of the
  same type, is serialized to its size as a variable-length unsigned
  integer (see below), followed by serializations of that many elements in
  this format, in the same order, without inter-padding or end marks.
  Examples of such sequences include arrays, STL containers.

- A heterogeneous static-length sequence, whose length is known from its
  type but may have elements of different types, is serialized to
  serializations of all its elements, in the same order, without
  inter-padding or end marks.  Examples of such sequences include
  `std::pair`, `std::tuple`.

- A heterogeneous tagged sequence, defined as an unordered set of elements
  each with a positive 8-bit integer tag, possibly of different types, is
  serialized to all of the elements in undefined order, each preceded by
  its tag represented in a single byte, then a variable-length unsigned
  integer of the serialized length of this member (excluding the tag or
  this length number) and finally ended with a byte of all zeros.  Such
  sequences may be used for custom classes to provide forward and/or
  backward compatibility in schema.

Note: this format does not list associative array as a supported type.
Such a construct is recommended to be serialized as a homogeneous
dynamic-length sequence, containing elements as the key-value pair
represented as a heterogeneous static-length sequence.

### Variable length unsigned integer encoding

This encoding writes an unsigned integer as bytes in little endian order,
in each byte the low 7 bits (`&0x7f`) composes up the integer.  The top bit
(`&0x80`) being 1 means the integer is not yet finished, and 0 means the
reverse.
