# Bamboo Shoot 3

[![Build Status](https://travis-ci.org/XeCycle/bamboo-shoot3.svg?branch=master)](https://travis-ci.org/XeCycle/bamboo-shoot3)

Bamboo Shoot 3 is designed for data persistence in PandaX without
dependency on ROOT.

## Rationale

At [PandaX Dark Matter Experiment](http://pandax.physics.sjtu.edu.cn) we
have used ROOT 5 from CERN for storing part of our data, but we have
encountered data format and ABI incompatibilities between patch versions of
ROOT.  ABI incompatibilities triggered runtime errors, where they could
have been link-time errors that can also be reported on program startup;
data format incompatibility is more annoying.  To eliminate these problems
we implemented the third generation of Bamboo Shoot from grounds up without
dependency on ROOT.

## Overview

This library is in 3 parts: object serialization library, named pbss; file
IO library, named pbsf; and support utilities, named pbsu.  They live in
namespace `pbss`, `pbsf` and `pbsu`.

The serialization library has an [open data format](doc/serialization.md)
which will stay unchanged throughout the lifetime of this third generation;
also it is planned to have implementations in multiple languages, currently
only in C++.  It supports tree-like data structures, and provides
forward/backward-compatibility on fields of custom structs by numerically
tagging each field.  The implementation currently runs at a speed
comparable to a one-byte-at-a-time copy.  Documentation at
[doc/pbss.md](doc/pbss.md).

The file IO library also has a stable [data format](doc/file.md), providing
simple sequential access and rudimentary support for random access.  The
two parts are built around the assumptions that a single stored object can
be large (typically 100kB~10MB), and sequential access is the primary
access pattern.  Documentation at [doc/pbsf.md](doc/pbsf.md).

The utilities library provides some simple utilities following functional
style, documented in [doc/utils.md](doc/utils.md).

## Installing and using

GCC >=4.8 or clang >=3.5 is required to compile the main code, and
[stack](http://haskellstack.org/) and [cmake](https://cmake.org/) are
required to build everything.  x86_64 GNU/Linux is the primary supported
platform, but it should work on OS X too.

Building from source can be done by
```
cmake -DCMAKE_BUILD_TYPE=<see below> /path/to/source
make install
```

If the installation is intended for local use only, `Native` build type is
recommended; other options are `AVX_release` for AVX-capable CPUs,
`Release` for general binary, or `Debug` for a debug build.

The library code is clean to
`-Wall -Wpedantic -Wextra -Wconversion -Werror`.

The whole library can be included by `#include <bs3/pbs.hh>`, and links to
a single library `libpbsf.a` or `libpbsf_s.so`.  Static linking is
recommended, which is what we do at PandaX.

## Versioning scheme

This library uses integer versioning, with an "r" prefix, e.g. "r123", and
pre-releases like "r123-pre", "r124-pre2".  That "3" in "Bamboo Shoot 3"
means "the third generation of Bamboo Shoot" and is not part of version
number.

We tend to break ABI compatibility aggressively, especially in pbsu and
pbsf; but whenever an ABI-incompatible change is introduced, the ABI
version tag on inline namespaces will be bumped, also the soversion of
`libpbsf_s.so` if it is related to pbsf.

## Licensing

The library is licensed under
[GPLv3 or later](http://www.gnu.org/licenses/gpl.html), and documentation
under [CC0](https://creativecommons.org/publicdomain/zero/1.0/) (public
domain).
