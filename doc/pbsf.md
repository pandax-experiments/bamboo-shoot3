# pbsf --- File format library

Library to read and write the file format.  Use of this library needs to
link to `libpbsf.a`, which is a CMake target named `pbsf`.

All functions and classes are in namespace `pbsf`.

## Errors

Errors thrown by the library:

- `class unknown_encoding_error : public std::runtime_error`
- `class bad_checksum_error : public std::runtime_error`
- `class unknown_realm_error : public std::runtime_error`

The names are pretty much self-explanatory.

## Realms

See [documentation for file format](file.md) for a definition of realm.  In
this library, realms are special types created by macro
`PBSF_DECLARE_REALM`.

### `PBSF_DECLARE_REALM`

Used like:
```cpp
PBSF_DECLARE_REALM(
  RealmName, 1234, /* realm id */
  PBSF_REGISTER_TYPE(1 /* type id */, Type1),
  PBSF_REGISTER_TYPE(3, Type2),
  ...
);
```

Semicolon at last is necessary.

This defines a struct called `RealmName`, has realm ID `1234`, and contains
all listed types identified by corrensponding type ID.

Defined struct inherits from some types in `pbsf` namespace, so realm
objects add in namespace `pbsf` for
[argument-dependent lookup](http://en.cppreference.com/w/cpp/language/adl).

### `lookup_type<id>(realm)`

`<Int> (Realm) -> Type`

Declares its return type as the one registered in `realm` as `id`.  It is
only declared, and is not defined with a function body so must not be
called unless inside a `decltype` expression.

### `lookup_id<type>(realm)`

`<Type> (Realm) -> Int`

Return type ID registered in `realm` for given `type`.  This is defined and
can be called.  Also, you need to actually call it to get the return value.

## Base API

The base API is low-level and not recommended for use, so leaving
undocumented at this time.  Use either sequential files or indexed files.

## Sequential files

### `open_sequential_input_file(filename, realm)`

`(String, Realm) -> sequential_file<std::ifstream, Realm>`

Open an input file in `realm`.  Throws `unknown_realm_error` if realm
mismatches.  Currently a mismatch in the magic number also throws this
error, but may be changed in the future.

### `open_sequential_output_file(filename, realm)`

`(String, Realm) -> sequential_file<std::ofstream, Realm>`

Open an output file in `realm`.  Create if not exists, truncate if exists.
Writes header automatically.

### `struct sequential_file<Stream, Realm>`

Represents a sequential file using `Stream` as underlying stream, and in
realm `Realm`.

- `.read_one_type<Type>()`: `<Type> () -> [Type]`, read values of `Type`
  from the file, skipping mismatched types.  Lazy except for the first
  read.
- `.write_iterator()`: `() -> OutputIterator<a>`, an output iterator that
  accepts any type registered in `Realm`.

## Indexed files

By placing a simple index at end of file we provide some rudimentary
support for random access, as indexed files.  All indexed files can be used
as sequential files with the same contents (but without keys, of course).

### `open_indexed_input_file<key_type>(filename, realm)`

Open an input file in `realm`, sorted by `key_type`, fails if not exists.
Throws: `unknown_realm_error` if realm does not match; `key_mismatch_error`
if `key_type` does not match; `type_mismatch_error` if metadata blocks does
not match; and maybe other errors from pbss if any errors occurred during
parsing.

### `open_indexed_output_file<key_type>(filename, realm, overwrite=true)`

Open an output file in `realm`, sorted by `key_type`; if `overwrite` is
true then truncate an existing file, append otherwise.  May throw the same
errors as `open_indexed_input_file` when reading from an existing file.  An
indexed output file is also readable.

### `file.{[c|r|cr]begin,[c|r|cr]end}`

Conventional STL container iterator interface.  `const_iterator` is the
same as `iterator`, and `reverse_iterator` is the same as
`const_reverse_iterator`.

The iterators are bidirectional, and have `value_type`
`std::pair<key_type, pbsu::lazy_value</*unspecified*/>>`.
Read from the file is performed when the `lazy_value` is dereferenced, and
returns an object of
```cpp
struct /*name unspecified*/ {
  template <class T> bool is() const;
  template <class T> T as();
};
```
where `.is<T>()` checks whether the contained object is of type `T`, and
`.as<T>()` parses the object as type `T`, throwing `type_mismatch_error` if
type does not match.  `as` can be used only once.

### `file.{find,lower_bound,upper_bound,size}`

Same as that of `std::map`.

### `file[key]`

This is a const member function and does not create a new item if `key` is
already there; instead it throws `key_missing_error`.  Returns the same
`pbsu::lazy_value` as iterators `.second`.

### `file.insert(tuple)`

Insert or replace an entry into `file`, using `std::get<0>(tuple)` as key
and `std::get<1>(tuple)` as value.

### `file.write_iterator()`

An output iterator that does the same thing as `insert`.  Accepts input in
a variadic manner.
