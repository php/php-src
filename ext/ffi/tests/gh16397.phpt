--TEST--
GH-16397 (Segmentation fault when comparing FFI object)
--EXTENSIONS--
ffi
--FILE--
<?php
$ffi = FFI::cdef();
try {
    var_dump($ffi != 1);
} catch (FFI\Exception $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
?>
--EXPECT--
FFI\Exception: Cannot compare FFI objects
