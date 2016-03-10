# Utilities

Bamboo Shoot 3 comes with some general utilities for use with its own types
and other programming.  This briefly documents it.

## Namespace

All functions and classes are in namespace `pbsu`.

## About type signatures

Functions are documented with a brief type signature that is similar to
what is used in Haskell; but there is no strict syntax on the signature,
and any representation that easily makes sense is used.  Some basic rules
are:

- `arg_type -> ret_type` denotes a function;
- type names beginning in lower case letters, e.g. `a` are unspecified
  types, and we are temporarily giving it a name, say `a` in this case;
- type names beginning in upper case letters are concrete types, or
  concepts --- e.g. `Tuple<a, b>` means a `std::tuple` of `a` and `b`,
  `Iterator<a>` means any type satisfying the `Iterator` concept, but `a`
  and `b` are not specified;
- square brackets `[a]` denote an "iterable" (which is something that can
  be looped over by `for (auto x : iterable)`) containing type `a`; again,
  `a` is not specified;
- functions of multiple or variadic arguments are expressed like `(a, b) ->
  c` or `(a...) -> b`; in the latter case, each type in `a...` may be
  different.

## Range basics

(Possibly) lazy ranges, packing a pair of
[iterators](http://en.cppreference.com/w/cpp/concept/Iterator).

### `struct range<Iterator>`

A pack of two iterators.  `.begin()` and `.end()` returns the begin and
end.  Has `.size()`, `.empty()`, `operator bool` (true if non-empty) and
`operator[]` if supported by underlying iterator.

Usually you need not explicitly create such a struct; use the helper `make_range`

### `make_range(iterable), make_range(begin, end)`

Overloads:
- `[a] -> [a]`: wraps any iterable (e.g. C array, `std::vector`,
  `std::map`, ...) into a `struct range`.
- `(Iterator<a>, Iterator<a>) -> [a]`: wrap a pair of iterators into a
  `struct range`.
  
Being a function, it can deduce parameter types.  E.g.:
```cpp
int a[3];
std::vector<double> b;
make_range(a); // same as a
make_range(b); // same as b
make_range(a+1, a+3); // contains a[1], a[2]
```

## Iterator utilities

### `transformed_writer(f, output_iter)`

`((a -> b), OutputIterator<b>) -> OutputIterator<a>`

Transform input value by given `f` then write to given `output_iter`.

### `multiplex_writer(outputs...)`

`(OutputIterator<a>...) -> OutputIterator<a>`

Return an `OutputIterator` that writes input value unchanged into all given
`outputs...`.

### `spread_writer(outputs...)`

`(OutputIterator<a>...) -> OutputIterator<Tuple<a...>>`

Return an `OutputIterator` taking `Tuple`s, that writes 1st value to 1st
iterator, 2nd value to 2nd iterator, ...

### `filtering_writer(predicate, output_iter)`

`((a -> Bool), OutputIterator<a>) -> OutputIterator<a>`

If given `predicate` returns true, write value to given `output_iter`.
Otherwise do nothing.

### `make_mapping_iterator(f, it)`

`((a -> b), Iterator<a>) -> Iterator<b>`

Map values from `it` by given `f`.

This is used for input.

### `make_filtering_iterator(predicate, begin, end)`

`((a -> Bool), Iterator<a>, Iterator<a>) -> Iterator<a>`

If `predicate` returns true, use this value.  Otherwise skip over it.

To make a corresponding end iterator, use
`make_filtering_iterator(predicate, end, end)`.  Note that you usually do
not need to do this: there is `filter`.

### `struct integral_iterator<class Int>`

Random access iterator incremented/decremented of step length `1`.
Dereferences to the corresponding integer.

## Range utilities

### `copy(range, output_iter)`

`([a], OutputIterator<a>) -> OutputIterator<a>`

Write values in `range` to `output_iter`.  Return the final `output_iter`
at last, same as
[`std::copy`](http://en.cppreference.com/w/cpp/algorithm/copy).

### `zip(range...)`

`([a]...) -> [Tuple<a...>]`

Combine multiple ranges into one range, containing tuples of values from
each range.  Each input range is consumed in parallel one-by-one.

Lazy.

### `map(f, range)`

`((a -> b), [a]) -> [b]`

Map `range` by `f` --- returned range contains values as returned by
`f(value_in_range)`.

This is lazy and guarantees ordered execution for all values till the last
used one (if an iterator is incremented before it was ever dereferenced, it
will be dereferenced first to ensure application of `f`).  For a version
that skips execution on unused values, use `map_skipping`; if `f` returns
equivalent values for same input, there is also `map_ephemeral` which does
not store the results of `f` and returns a prvalue.  Note that the
ephemeral one can be more restricted that you think --- if `f` returns
containers, then usually two results are not equivalent, in that an
iterator to the first result and an iterator to the second result cannot be
used together.

### `map_indexed(f, range)`

Map `range` by `f` with index applied as `f(value, index)`.  This uses the
default version of `map` and there is no other variants at present.

### `filter(predicate, range)`

`((a -> Bool), [a]) -> [a]`

Filter `range` to what `predicate` returns true.

Lazy.

### `reduce(f, init, range), reduce(f, range)`

Left fold of `range` using `f`.  Uses
[`std::accumulate`](http://en.cppreference.com/w/cpp/algorithm/accumulate).

Overloads:
- `(((a, b) -> a), a, [b]) -> a`: uses given `init` as initial value;
- `(((a, b) -> a), [b]) -> a`: uses first value in `range` as initial
  value, throws
  [`std::out_of_range`](http://en.cppreference.com/w/cpp/error/out_of_range)
  if `range` is empty.
  
## Tuple utilities

### `spread_call(f, tuple)`

`(((a...) -> b), Tuple<a...>) -> b`

Call `f` with arguments from `tuple`.

### `pick_call<index...>(f, tuple)`

`<Int...> (((a...) -> b), Tuple<superset_of_a...>) -> b`

Call `f` with arguments from `tuple` specified by given `index...`.

### `map_tuple(f, tuple)`

`({a -> b ...}, Tuple<a...>) -> Tuple<b...>`

Here `{a -> b ...}` means that f may be an object with multiple overloaded
`operator()` or templated `operator()` that can be called with different
types and return possibly different types.

Call `f` with each value in `tuple`, collect the result into a tuple.

## Other functional utilities

### `struct constref_forward` (a.k.a `identity`)

A `constref_forward()` has an `operator()` that takes any value by constant
reference, return it unchanged in constant reference.

### `reverse_call(f, args...)`

`(((a...) -> b), a...) -> b`

Call `f` with `args...` in reverse order.

### `compose(f...)`

`(y -> z, ..., b -> c, a -> b) -> (a -> z)`

Right-to-left function composition: returned function calls rightmost
function first, then apply its return value to second rightmost function,
and so on.  Return the return value of first function.

### `pipe(f...)`

`(a -> b, b -> c, ..., y -> z) -> (a -> z)`

Left-to-right function composition, similar to `compose`.

### `spread(f)`

`(((a...) -> b)) -> (Tuple<a...> -> b)`

`spread(f)(tuple)` is equivalent to `spread_call(f, tuple)`.  It is like
partially-applied `spread_call` with only the function.

### `pick<index...>(f)`

`<Int...> (((a...) -> b)) -> (Tuple<superset_of_a...> -> b)`

Similar to `spread` but used `pick_call`.

### `call_with(value)`

`a -> ((a -> b) -> b)`

`call_with(value)(f)` is equivalent to `f(value)`.

### `collect_result(f...)`

`((a -> b)...) -> (a -> Tuple<b...>)`

Returned function applies given value to each `f...`, collect their results
into a tuple.

## Miscellaneous

### `to_unsigned`, `to_signed`

Convert a number to its signed/unsigned type of same size.  If
`defined(DEBUG) || !defined(NDEBUG)`, performs out-of-range checking and
throws `std::out_of_range` if the result would be incorrect.

### `struct lazy_value<Computation>`

Performs `Computation` at most once, when requested by `operator*`.  Has
`computation` as its public data member.  Can be default constructed, or
constructed from an instance of `Computation`.
