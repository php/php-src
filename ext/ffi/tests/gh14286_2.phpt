--TEST--
GH-14286 (ffi enum type (when enum has no name) make memory leak)
--EXTENSIONS--
ffi
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
