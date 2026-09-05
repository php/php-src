# Bundled Extensions

```{toctree}
   :hidden:

bundled-extensions/filter
```

Bundled extensions are maintained in `ext/` as part of php-src.

## Extension Developers

The files which are to be compiled are specified in `config.m4` using the
following macro:

```text
PHP_REQUIRE_CXX()
PHP_NEW_EXTENSION([foo], [foo.c bar.c baz.cpp], [$ext_shared],,, [cxx])
```

E.g. this enables the extension foo which consists of three source-code modules,
two in C and one in C++. And, depending on the user's wishes, the extension will
even be built as a dynamic module. `PHP_REQUIRE_CXX` initialises the C++
toolchain, and the `cxx` argument makes a shared extension use the C++ linker.

The full syntax:

```text
PHP_NEW_EXTENSION(extname, sources [, shared [, sapi_class [, extra-cflags [, cxx [, zend_ext]]]]])
```

Please have a look at `build/php.m4` for the gory details and meanings of the
other parameters.

And that's basically it for the extension side.

If you would otherwise build sub-libraries for this module, add the source-code
files here as well. If you need to specify separate include directories, do it
this way:

```text
PHP_NEW_EXTENSION([foo], [foo.c mylib/bar.c mylib/gregor.c],,, [-I@ext_srcdir@/lib])
```

E.g. this builds the three files which are located relative to the extension
source directory and compiles all three files with the special include directive
(`@ext_srcdir@` is automatically replaced).

Now, you need to tell the build system that you want to build files in a
directory called `$ext_builddir/lib`:

```text
PHP_ADD_BUILD_DIR([$ext_builddir/lib])
```

Make sure to call this after `PHP_NEW_EXTENSION`, because `$ext_builddir` is
only set by the latter.

If you have a complex extension, you might need to add special Make rules. You
can do this by calling `PHP_ADD_MAKEFILE_FRAGMENT` in your `config.m4` after
`PHP_NEW_EXTENSION`.

This will read a file in the source-dir of your extension called
`Makefile.frag`. In this file, `$(builddir)` and `$(srcdir)` will be replaced by
the values which are correct for your extension and which are again determined
by the `PHP_NEW_EXTENSION` macro.

Make sure to prefix *all* relative paths correctly with either `$(builddir)` or
`$(srcdir)`. Because the build system does not change the working directory
anymore, we must use either absolute paths or relative ones to the top
build-directory. Correct prefixing ensures that.

## General Info

The foundation for the build system is the flexible handling of sources and their
contexts. With the help of macros you can define special flags for each
source-file, where it is located, in which target context it can work, etc.

Have a look at the well documented `PHP_ADD_SOURCES` macro in `build/php.m4`.
