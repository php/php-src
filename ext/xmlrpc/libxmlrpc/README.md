# libxmlrpc

This is a fork of the [xmlrpc-epi library](http://xmlrpc-epi.sourceforge.net/)
written by Dan Libby.

## Original coding conventions

Organization of this directory is moving towards this approach:

* `<module>.h`         -- public API and data types
* `<module>_private.h` -- protected API and data types
* `<module>.c`         -- implementation and private API / types

The rules are:

* `.c` files may include `*_private.h`.
* `.h` files may not include `*_private.h`

This allows us to have a nicely encapsulated C api with opaque data types and
private functions that are nonetheless shared between source files without
redundant extern declarations..
