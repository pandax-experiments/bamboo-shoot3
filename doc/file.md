# Bamboo Shoot 3 file format

This document defines how a sequence of serialized objects (possibly of
different types) is mapped to a sequence of bytes.

## Concepts

- A file is a sequence of bytes that can be read from the beginning in a
  forward-only manner, and/or can be written to the end in an append-only
  manner.  Other features are optional, and an implementation may utilise
  such features where applicable, but must not write in any behavior other
  than appending to the end.
- A realm is a set of possible Content-Types that may be contained in a
  single file.

## Format

The bytes in a file constitute a file header followed by zero or more data
blocks, in an order specified by the implementation.

### File header

A file header is:

- Four bytes of the ASCII representation of "pbs3", followed by
- Four bytes of any character, as the identifier of the realm of this file.

The first four bytes is the magic number; the realm is application-specific
and can be any value.  An implementation must report an error if the realm
cannot be handled.

### Data block

A data block is:

- A signed 16-bit integer as Content-Type,
- A signed 16-bit integer as Content-Encoding,
- An unsigned 32-bit integer as Content-Checksum,
- A variable length unsigned integer as Content-Length,
- Exactly Content-Length bytes of implementation-specific data,

laid out sequentially without any padding or end marks, where types that
are not bytes are serializations in the serialization format of Bamboo
Shoot 3 object serialization.

Content-Type is application- and implementation-specific, where the
application should use non-negative values and the implementation should
use negative values for internal types.  Content-Encoding is
implementation-specific.  Content-Checksum is the CRC-32C checksum of the
bytes of implementation-specific data; the checksum is computed for the
final stored bytes in the file, after implementation-defined encoding takes
place.

## Compatibility requirements

An implementation must:

- allow the application to deal with only known types in known realms,
  without causing critical errors, unless stated otherwise;
- report an error if the realm is not known;
- refuse to read from and/or write to a file of unknown realm.

An implementation may:

- report an error for a unknown Content-Type from a known realm, but must
  be able to continue normal operation, regardless whether the unknown
  Content-Type is application defined or implementation defined;
- cause a critical error and stop operations on a file if one of the
  following conditions met, but it shall be able to continue normal
  operations on other files:
  * the file in question ended in the middle of a file header or a data
  block;
  * a data block in the file has a mismatched checksum;
  * one or more data blocks required by the implementation is missing from
  the file or is broken.
- assume a block of a negative Content-Type unknown to the implementation
  is not handled by the application, and do whatever is reasonable,
  including but not limited to ignoring it without passing to the
  application.
