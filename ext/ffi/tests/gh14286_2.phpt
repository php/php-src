--TEST--
GH-14286 (ffi enum type (when enum has no name) make memory leak)
--EXTENSIONS--
ffi
--SKIPIF--
<?php
if (PHP_DEBUG || getenv('SKIP_ASAN')) die("xfail: FFI cleanup after parser error is nor implemented");
?>
--INI--
ffi.enable=1
--FILE--
<?php
try {
    $ffi = FFI::cdef("
        enum {
            TEST_ONE=1,
            TEST_TWO=2,
        } x;
    ");
} catch (Throwable $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECT--
Failed resolving C variable 'x'
