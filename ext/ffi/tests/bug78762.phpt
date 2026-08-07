--TEST--
Bug #78762 (Failing FFI::cast() may leak memory)
--EXTENSIONS--
ffi
--FILE--
<?php
try {
    FFI::cdef()->cast('char[10]', FFI::cdef()->new('char[1]'));
} catch (FFI\Exception $ex) {
    echo $ex::class, ': ', $ex->getMessage(), PHP_EOL;
}
?>
--EXPECT--
FFI\Exception: attempt to cast to larger type
