--TEST--
Definitions should not leak when using FFI::cdef()->new(...)
--EXTENSIONS--
ffi
--FILE--
<?php
$struct = \FFI::cdef()->new('struct Example { uint32_t x; }');
var_dump($struct);
?>
--EXPECT--
object(FFI\CData:struct Example)#2 (1) {
  ["x"]=>
  int(0)
}
