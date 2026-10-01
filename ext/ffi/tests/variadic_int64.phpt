--TEST--
FFI passes integer variadic arguments as int64_t with a 64-bit zend_long
--EXTENSIONS--
ffi
zend_test
--SKIPIF--
<?php
if (PHP_INT_SIZE !== 8) {
    die("skip only for a 64-bit zend_long");
}
?>
--FILE--
<?php
$ffi = FFI::cdef('void bug_gh9090_void_int_char_var(int i, char *fmt, ...);');

$ffi->bug_gh9090_void_int_char_var(0, "%lld %lld %lld", 1, 2, 3);
$ffi->bug_gh9090_void_int_char_var(0, "%lld %s", -1, "ok");
$ffi->bug_gh9090_void_int_char_var(0, "%lld %lld %s", 2 ** 32 + 1, -(2 ** 32) - 1, "ok");
$ffi->bug_gh9090_void_int_char_var(0, "%lld %lld %s", PHP_INT_MAX, PHP_INT_MIN, "ok");
$ffi->bug_gh9090_void_int_char_var(0, "%s %lld %.1f %lld", "ok", 1, 2.5, 3);
?>
--EXPECT--
bug_gh9090_void_int_char_var 1 2 3
bug_gh9090_void_int_char_var -1 ok
bug_gh9090_void_int_char_var 4294967297 -4294967297 ok
bug_gh9090_void_int_char_var 9223372036854775807 -9223372036854775808 ok
bug_gh9090_void_int_char_var ok 1 2.5 3
