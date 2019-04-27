# FFI PHP extension (Foreign Function Interface)

FFI PHP extension provides a simple way to call native functions, access native
variables and create/access data structures defined in C language. The API of
the extension is very simple and demonstrated by the following example and its
output.

```php
<?php
$libc = FFI::cdef("
    int printf(const char *format, ...);
    const char * getenv(const char *);
    unsigned int time(unsigned int *);

    typedef unsigned int time_t;
    typedef unsigned int suseconds_t;

    struct timeval {
        time_t      tv_sec;
        suseconds_t tv_usec;
    };

    struct timezone {
        int tz_minuteswest;
        int tz_dsttime;
    };

    int gettimeofday(struct timeval *tv, struct timezone *tz);
", "libc.so.6");

$libc->printf("Hello World from %s!\n", "PHP");
var_dump($libc->getenv("PATH"));
var_dump($libc->time(null));

$tv = $libc->new("struct timeval");
$tz = $libc->new("struct timezone");
$libc->gettimeofday(FFI::addr($tv), FFI::addr($tz));
var_dump($tv->tv_sec, $tv->tv_usec, $tz);
?>
```

```txt
Hello World from PHP!
string(135) "/usr/lib64/qt-3.3/bin:/usr/lib64/ccache:/usr/local/bin:/usr/bin:/bin:/usr/local/sbin:/usr/sbin:/home/dmitry/.local/bin:/home/dmitry/bin"
int(1523617815)
int(1523617815)
int(977765)
object(FFI\CData:<struct>)#3 (2) {
  ["tz_minuteswest"]=>
  int(-180)
  ["tz_dsttime"]=>
  int(0)
}
```

`FFI::cdef()` takes two arguments (both are optional). The first one is a
collection of C declarations and the second is DSO library. All variables and
functions defined by first arguments are bound to corresponding native symbols
in DSO library and then may be accessed as FFI object methods and properties. C
types of argument, return value and variables are automatically converted
to/from PHP types (if possible). Otherwise, they are wrapped in a special CData
proxy object and may be accessed by elements.

In some cases (e.g. passing C structure by pointer) we may need to create a real
C data structures. This is possible using `FFF::new()` method. It takes a C type
definition and may reuse C types and tags defined by `FFI::cdef()`.

It's also possible to use `FFI::new()` as a static method to create arbitrary C
data structures.

```php
<?php
$p = FFI::new("struct {int x,y;} [2]");
$p[0]->x = 5;
$p[1]->y = 10;
var_dump($p);
```

```txt
object(FFI\CData:<struct>[2])#1 (2) {
  [0]=>
  object(FFI\CData:<struct>)#2 (2) {
    ["x"]=>
    int(5)
    ["y"]=>
    int(0)
  }
  [1]=>
  object(FFI\CData:<struct>)#3 (2) {
    ["x"]=>
    int(0)
    ["y"]=>
    int(10)
  }
}
```

## API reference

### function FFI::cdef([string $cdef = "" [, string $lib = null]]): FFI

#### Call native functions

All functions defined in `FFI::cdef()` may be called as methods of the created
FFI object.

```php
$libc = FFI::cdef("const char * getenv(const char *);", "libc.so.6");
var_dump($libc->getenv("PATH"));
```

#### Read/write values of native variables

All functions defined in `FFI::cdef()` may be accessed as properties of the
created FFI object.

```php
$libc = FFI::cdef("extern int errno;", "libc.so.6");
var_dump($libc->errno);
```

### function FFI::type(string $type): FFI\CType

This function creates and returns a `FFI\CType` object, representng type of
the given C type declaration string.

`FFI::type()` may be called statically and use only predefined types, or as a
method of previously created FFI object. In last case the first argument may
reuse all type and tag names defined in `FFI::cdef()`.

### function FFI::typeof(FFI\CData $type): FFI\CType

This function returns a `FFI\CType` object, representing the type of the given
`FFI\CData` object.

### static function FFI::arrayType(FFI\CType $type, array $dims): FFI\CType

Constructs a new C array type with elements of $type and dimensions specified by
$dims.

### function FFI::new(mixed $type [, bool $own = true [, bool $persistent = false]]): FFI\CData

This function may be used to create a native data structure. The first argument
is a C type definition. It may be a `string` or `FFI\CType` object. The
following example creates two dimensional array of integers.

```php
$p = FFI::new("int[2][2]");
var_dump($p, FFI::sizeof($p));
```

`FFI::new()` may be called statically and use only predefined types, or as a
method of previously created FFI object. In last case the first argument may
reuse all type and tag names defined in `FFI::cdef()`.

By default `FFI::new()` creates "owned" native data structures, that live
together with corresponding PHP object, reusing PHP reference-counting and GC.
However, in some cases it may be necessary to manually control the life time of
the data structure. In this case, the PHP ownership on the corresponding data,
may be manually changed, using `false` as the second optianal argument. Later,
not-owned CData should be manually deallocated using `FFI::free()`.

Using the optional $persistent argument it's possible to allocate C objects in
persistent memory, through `malloc()`, otherwise memory is allocated in PHP
request heap, through `emalloc()`.

### static function FFI::free(FFI\CData $cdata): void

Manually removes previously created "not-owned" data structure.

#### Read/write elements of native arrays

Elements of native array may be accessed in the same way as elements of PHP
arrays. Of course, native arrays support only integer indexes. It's not possible
to check element existence using `isset()` or `empty()` and remove element using
`unset()`. Native arrays work fine with "foreach" statement.

```php
$p = FFI::new("int[2]");
$p[0] = 1;
$p[1] = 2;
foreach ($p as $key => $val) {
    echo "$key => $val\n";
}
```

#### Read/write fields of native "struct" or "union"

Fields of native struct/union may be accessed in the same way as properties of
PHP objects. It's not possible to check filed existence using `isset()` or
`empty()`, remove them using `unset()`, and iterate using "foreach" statement.

```php
$pp = FFI::new("struct {int x,y;}[2]");
foreach($pp as $n => &$p) {
    $p->x = $p->y = $n;
}
var_dump($pp);
```

#### Pointer arithmetic

CData pointer values may be incremented/decremented by a number. The result is a
pointer of the same type moved on given offset.

Two pointers to the same type may be subtracted and return difference (similar
to C).

### static function FFI::sizeof(mixed $cdata_or_ctype): int

Returns size of C data type of the given `FFI\CData` or `FFI\CType`.

### static function FFI::alignof(mixed $cdata_or_ctype): int

Returns size of C data type of the given `FFI\CData` or `FFI\CType`.

### static function FFI::memcpy(FFI\CData $dst, mixed $src, int $size): void

Copies $size bytes from memory area $src to memory area $dst. $src may be any
native data structure (`FFI\CData`) or PHP `string`.

### static function FFI::memcmp(mixed $src1, mixed $src2, int $size): int

Compares $size bytes from memory area `$src1` and `$dst2`. `$src1` and `$src2`
may be any native data structures (`FFI\CData`) or PHP `string`s.

### static function FFI::memset(FFI\CData $dst, int $c, int $size): void

Fills the $size bytes of the memory area pointed to by `$dst` with the constant
byte `$c`.

### static function FFI::string(FFI\CData $src [, int $size]): string

Creates a PHP string from `$size` bytes of memory area pointed by `$src`. If
size is omitted, $src must be zero terminated array of C chars.

### function FFI::cast(mixed $type, FFI\CData $cdata): FFI\CData

Casts given $cdata to another C type, specified by C declaration `string` or
`FFI\CType` object.

This function may be called statically and use only predefined types, or as a
method of previously created FFI object. In last case the first argument may
reuse all type and tag names defined in `FFI::cdef()`.

### static function addr(FFI\CData $cdata): FFI\CData

Returns C pointer to the given C data structure. The pointer is not "owned" and
won't be free. Anyway, this is a potentially unsafe operation, because the
life-time of the returned pointer may be longer than life-time of the source
object, and this may cause dangling pointer dereference (like in regular C).

### static function load(string $filename): FFI

Instead of embedding of a long C definition into PHP string, and creating FFI
through `FFI::cdef()`, it's possible to separate it into a C header file. Note,
that C preprocessor directives (e.g. `#define` or `#ifdef`) are not supported.
And only a couple of special macros may be used especially for FFI.

```c
#define FFI_LIB "libc.so.6"

int printf(const char *format, ...);
```

Here, FFI_LIB specifies, that the given library should be loaded.

```php
$ffi = FFI::load(__DIR__ . "/printf.h");
$ffi->printf("Hello world!\n");
```

### static function scope(string $name): FFI

FFI definition parsing and shared library loading may take significant time.
It's not useful to do it on each HTTP request in WEB environment. However, it's
possible to pre-load FFI definitions and libraries at php startup, and
instantiate FFI objects when necessary. Header files may be extended with
`FFI_SCOPE` define (default pre-loading scope is "C"). This name is going to
be used as `FFI::scope()` argument. It's possible to pre-load few files into a
single scope.

```c
#define FFI_LIB "libc.so.6"
#define FFI_SCOPE "libc"

int printf(const char *format, ...);
```

These files are loaded through the same `FFI::load()` load function, executed
from file loaded by `opcache.preload` php.ini directive.

```ini
ffi.preload=/etc/php/ffi/printf.h
```

Finally, `FFI::scope()` instantiates an `FFI` object, that implements all C
definitions from the given scope.

```php
$ffi = FFI::scope("libc");
$ffi->printf("Hello world!\n");
```

## Owned and not-owned CData

FFI extension uses two kind of native C data structures. "Owned" pointers are
created using `FFI::new([, true])`, `clone`d. Owned data is deallocated
together with last PHP variable, that reference it. This mechanism reuses PHP
reference-counting and garbage-collector.

Elements of C arrays and structures, as well as most data structures returned by
C functions are "not-owned". They work just as regular C pointers. They may leak
memory, if not freed manually using `FFI::free()`, or may become dangling
pointers and lead to PHP crashes.

The following example demonstrates the problem.

```php
$p1 = FFI::new("int[2][2]"); // $p1 is owned pointer
$p2 = $p1[0];                // $p2 is not-owned part of $p1
unset($p1);                  // $p1 is deallocated ($p2 became dangling pointer)
var_dump($p2);               // crash because dereferencing of dangling pointer
```

It's possible to change ownership, to avoid this crash, but this would require
manual memory management and may lead to memory leaks.

```php
$p1 = FFI::new("int[2][2]", false); // $p1 is not-owned pointer
$p2 = $p1[0];
unset($p1);                         // $p1 CData is keep alive (memory leak)
var_dump($p2);                      // works fine, except of memory leak
```

## PHP callbacks

It's possible to assign PHP function to native function variable (or pass it as
a function argument). This seems to work, but this functionality is not
supported on all libffi platforms, it is not efficient and leaks resources by
the end of request.

## FFI API restriction

With FFI users may do almost anything, like in C, and therefor may crash PHP in
thousand ways. It's possible to completely disable or enable all FFI functions
using `ffi.enable=0/1` configuration directives, or limit FFI usage to preloaded
scripts using `ffi.enable=preload` (this is the default setting). In case FFI is
not completely disabled, it's also enabled for CLI scripts. Finally, the
restriction affects only FFI functions their selves, but not the overloaded
method of created FFI or CData objects.

## Status

In current state, access to FFI data structures is significantly (about 2 times)
slower, than access to PHP arrays and objects. It makes no sense to use them for
speed, but may make sense to reduce memory consumption.

FFI functionality may be included into PHP-8 core, to provide better
interpretation performance and integrate with JIT, providing almost C
performance (similar to LuaJIT).

## Requirement

* [libffi-3.*](https://sourceware.org/libffi/)

## Installation

```bash
./configure ... --with-ffi
make
sudo make install
```

## Real usage

FFI extension was used to implement
[PHP TensorFlow binding](https://github.com/dstogov/php-tensorflow).

## FFI parser

FFI C parser is generated using [LLK](https://github.com/dstogov/llk).
