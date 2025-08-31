--TEST--
Bug #77632 (FFI function pointers with variadics)
--EXTENSIONS--
ffi
--SKIPIF--
<?php
try {
    FFI::cdef("extern void *zend_printf;");
} catch (Throwable $_) {
    die('skip PHP symbols not available');
}
?>
--INI--
ffi.enable=1
--FILE--
<?php
$libc = FFI::cdef("extern size_t (*zend_printf)(const char *format, ...);");
$args = ["test from zend_printf\n"];
($libc->zend_printf)(...$args);
$args2 = ["Hello, %s from zend_printf\n", "world"];
($libc->zend_printf)(...$args2);
?>
--EXPECT--
test from zend_printf
Hello, world from zend_printf
