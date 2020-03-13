--TEST--
Bug #77632 (FFI function pointers with variadics)
--SKIPIF--
<?php
require_once('skipif.inc');
require_once('utils.inc');
try {
	FFI::cdef("extern void *zend_printf;", ffi_get_php_dll_name());
} catch (Throwable $_) {
	die('skip PHP symbols not available');
}
?>
--INI--
ffi.enable=1
--FILE--
<?php
require_once('utils.inc');
$libc = FFI::cdef("extern size_t (*zend_printf)(const char *format, ...);", ffi_get_php_dll_name());
$args = ["test from zend_printf\n"];
($libc->zend_printf)(...$args);
$args2 = ["Hello, %s from zend_printf\n", "world"];
($libc->zend_printf)(...$args2);
?>
--EXPECT--
test from zend_printf
Hello, world from zend_printf
