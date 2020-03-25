# PHP build system V5 overview

* supports Makefile.ins during transition phase
* not-really-portable Makefile includes have been eliminated
* supports separate build directories without VPATH by using explicit rules only
* does not waste disk-space/CPU-time for building temporary libraries =>
  especially noticeable on slower systems
* slow recursive make replaced with one global Makefile
* eases integration of proper dependencies
* adds PHP_DEFINE(what[, value]) which creates a single include-file per what.
  This will allow more fine-grained dependencies.
* abandoning the "one library per directory" concept
* improved integration of the CLI
* several new targets:
  * `build-modules`: builds and copies dynamic modules into `modules/`
  * `install-cli`: installs the CLI only, so that the install-sapi target does
    only what its name says
* finally abandoned automake
* changed some configure-time constructs to run at buildconf-time
* upgraded shtool to 1.5.4
* removed `$(moduledir)` (use `EXTENSION_DIR`)

## The reason for a new system

It became more and more apparent that there is a severe need for addressing the
portability concerns and improving the chance that your build is correct (how
often have you been told to `make clean`? When this is done, you won't need to
anymore).

## If you build PHP on a Unix system

You, as a user of PHP, will notice no changes. Of course, the build system will
be faster, look better and work smarter.

## If you are developing PHP

### Extension developers

Makefile.ins are abandoned. The files which are to be compiled are specified in
the `config.m4` now using the following macro:

```m4
PHP_NEW_EXTENSION(foo, foo.c bar.c baz.cpp, $ext_shared)
```

E.g. this enables the extension foo which consists of three source-code modules,
two in C and one in C++. And, depending on the user's wishes, the extension will
even be built as a dynamic module.

The full syntax:

```m4
PHP_NEW_EXTENSION(extname, sources [, shared [,sapi_class[, extra-cflags]]])
```

Please have a look at `build/php.m4` for the gory details and meanings of the
other parameters.

And that's basically it for the extension side.

If you previously built sub-libraries for this module, add the source-code files
here as well. If you need to specify separate include directories, do it this
way:

```m4
PHP_NEW_EXTENSION(foo, foo.c mylib/bar.c mylib/gregor.c,,,-I@ext_srcdir@/lib)
```

E.g. this builds the three files which are located relative to the extension
source directory and compiles all three files with the special include directive
(`@ext_srcdir@` is automatically replaced).

Now, you need to tell the build system that you want to build files in a
directory called `$ext_builddir/lib`:

```m4
PHP_ADD_BUILD_DIR($ext_builddir/lib)
```

Make sure to call this after `PHP_NEW_EXTENSION`, because `$ext_builddir` is
only set by the latter.

If you have a complex extension, you might to need add special Make rules. You
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

### SAPI developers

Instead of using `PHP_SAPI=foo/PHP_BUILD_XYZ`, you will need to type

```m4
PHP_SELECT_SAPI(name, type, sources.c)
```

I.e. specify the source-code files as above and also pass the information
regarding how PHP is supposed to be built (shared module, program, etc).

For example for APXS:

```m4
PHP_SELECT_SAPI(apache, shared, sapi_apache.c mod_php.c php_apache.c)
```

## General info

The foundation for the new system is the flexible handling of sources and their
contexts. With the help of macros you can define special flags for each
source-file, where it is located, in which target context it can work, etc.

Have a look at the well documented macros `PHP_ADD_SOURCES(_X)` in
`build/php.m4`.
