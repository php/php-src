--TEST--
GH-10568 (Assertion failure when var_dump'ing void FFI result)
--EXTENSIONS--
ffi
--SKIPIF--
<?php
try {
    $libc = FFI::cdef("void strlen(const char *s);", "libc.so.6");
} catch (Throwable $_) {
    die('skip libc.so.6 not available');
}
?>
--INI--
ffi.enable=1
--FILE--
<?php
// Yes, I know that strlen does not return void, but I need a simple & portable test case.
$libc = FFI::cdef("void strlen(const char *s);", "libc.so.6");
var_dump($libc->strlen("abc"));
?>
DONE
--EXPECT--
NULL
DONE
