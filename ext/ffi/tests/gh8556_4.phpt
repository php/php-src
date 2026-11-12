--TEST--
GH-8556 (FFI\CData use-after-free after FFI object destroyed) - type variation
--EXTENSIONS--
ffi
--SKIPIF--
<?php
try {
    $libc = FFI::cdef("char *getenv(char *);", "libc.so.6");
} catch (Throwable $_) {
    die('skip libc.so.6 not available');
}
?>
--INI--
ffi.enable=1
--FILE--
<?php
$a = FFI::cdef("typedef int T[5];");
$pointer = $a->type("T*");
$casted = $a->cast($pointer, $a->new("T"));
var_dump($pointer);
unset($a);
var_dump($pointer);
unset($pointer);
var_dump($casted);
?>
--EXPECT--
object(FFI\CType:int32_t(*)[5])#2 (0) {
}
object(FFI\CType:int32_t(*)[5])#2 (0) {
}
object(FFI\CData:int32_t(*)[5])#4 (1) {
  [0]=>
  NULL
}
