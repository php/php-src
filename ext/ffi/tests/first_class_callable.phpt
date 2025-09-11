--TEST--
Bug: Cannot create first-class callable from CData function
--EXTENSIONS--
ffi
--SKIPIF--
<?php
try {
    $libc = FFI::cdef("int printf(const char *format, ...);", "libc.so.6");
} catch (Throwable $_) {
    die('skip libc.so.6 not available');
}
?>
--INI--
ffi.enable=1
--FILE--
<?php
$libc = FFI::cdef("int printf(const char *format, ...);", "libc.so.6");
$libc->printf("Hello world\n");
$closure = $libc->printf;
$closure("%s %s\n", "Hello", "world");
$closure = $libc->printf(...);
$closure("%s %s\n", "Hello", "world");
?>
--EXPECT--
Hello world
Hello world
Hello world
