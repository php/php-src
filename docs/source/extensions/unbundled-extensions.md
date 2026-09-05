# Unbundled Extensions

An unbundled extension is maintained and distributed independently of php-src.
The PHP build system refers to these as self-contained extensions. To create
one, two things are required:

* Configuration file (`config.m4`)
* Source code for your module

We will describe now how to create these and how to put things together.

## Preparing Your System

A developer's setup needs these tools in addition to a C compiler and `make`:

* [GNU Autoconf](https://www.gnu.org/software/autoconf/)
* [GNU M4](https://www.gnu.org/software/m4/)

## Converting an Existing Extension

Just to show you how easy it is to create an unbundled extension, we will
convert a bundled extension into an unbundled one. Install PHP, including
its development tools and headers, and execute the following commands.

```bash
mkdir /tmp/newext
cd /tmp/newext
```

You now have an empty directory. We will copy the files from the dl_test
extension:

```bash
cp -R /path/to/php-src/ext/dl_test/. .
```

It is time to finish the module. Run:

```bash
phpize
```

The extension can now be built independently of the PHP source tree.

The user instructions boil down to

```bash
./configure \
    [--with-php-config=/path/to/php-config]
make
make test
make install
```

## Defining the New Extension

Our demo extension is called "foobar".

It consists of two source files `foo.c` and `bar.c` (and any arbitrary amount of
header files, but that is not important here).

The demo extension does not reference any external libraries (that is important,
because the user does not need to specify anything).

## Creating the M4 Configuration File

The m4 configuration can perform additional checks. For an unbundled
extension, you do not need more than a few macro calls.

```text
PHP_ARG_ENABLE([foobar],
  [whether to enable foobar],
  [AS_HELP_STRING([--enable-foobar],
    [Enable foobar])])

if test "$PHP_FOOBAR" != "no"; then
  PHP_NEW_EXTENSION([foobar], [foo.c bar.c], [$ext_shared])
fi
```

`PHP_ARG_ENABLE` will automatically set the correct variables, so that the
extension will be enabled by `PHP_NEW_EXTENSION` in shared mode.

The first argument of `PHP_NEW_EXTENSION` describes the name of the extension.
The second names the source-code files. The third passes `$ext_shared`, which is
set by `PHP_ARG_ENABLE` or `PHP_ARG_WITH`, to `PHP_NEW_EXTENSION`.

Please use always `PHP_ARG_ENABLE` or `PHP_ARG_WITH`. Even if you do not plan to
distribute your module with PHP, these facilities allow you to integrate your
module easily into the main PHP module framework.

## Creating Source Files

`ext_skel.php` creates a current extension skeleton, including configuration,
source, stub and test files. From the root of the PHP source tree, run:

```bash
php ext/ext_skel.php --ext foobar --vendor vendor_name
```

The generated source includes basic function definitions and an example of
handling function arguments. See `php ext/ext_skel.php --help` for further
information.

As for the rest, you are currently alone here. There are a lot of existing
modules, use a simple module as a starting point and add your own code.

## Creating the Unbundled Extension

Put `config.m4` and the source files into one directory. Then, run `phpize`
(this is installed during `make install` by PHP).

For example, if you configured PHP with `--prefix=/php`, you would run

```bash
/php/bin/phpize
```

This will automatically copy the necessary build files and create configure from
your `config.m4`.

And that's it. You now have an unbundled extension.

## Installing an Unbundled Extension

An extension can be installed by running:

```bash
./configure \
    [--with-php-config=/path/to/php-config]
make
make test
make install
```

## Adding Shared Module Support to a Module

In order to be useful, an unbundled extension must be loadable as a shared
module. The following will explain now how you can add shared module support to
an existing module called `foo`.

1. In `config.m4`, use `PHP_ARG_WITH/PHP_ARG_ENABLE`. Then you will
   automatically be able to use `--with-foo=shared[,DIR]` or
   `--enable-foo=shared`.

2. In `config.m4`, use `PHP_NEW_EXTENSION([foo], [foo.c], [$ext_shared])` to enable
   building the extension.

3. Add the following lines to your C source file:

```c
#ifdef COMPILE_DL_FOO
    ZEND_GET_MODULE(foo)
#endif
```
