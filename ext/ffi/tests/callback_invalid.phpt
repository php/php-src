--TEST--
FFI: assigning an invalid callback
--EXTENSIONS--
ffi
--INI--
ffi.enable=1
--FILE--
<?php
$ffi = FFI::cdef('typedef int (*fn)(void);');
$p = FFI::addr($ffi->new('fn'));
try {
    $p[0] = 'nope';
} catch (FFI\Exception $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
?>
--EXPECT--
FFI\Exception: Attempt to assign an invalid callback, function "nope" not found or invalid function name
