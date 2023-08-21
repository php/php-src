--TEST--
FFI 006: Pointer assignment
--EXTENSIONS--
ffi
--INI--
ffi.enable=1
--FILE--
<?php
$ffi = FFI::cdef();

$v = $ffi->new("int*[2]");
$v[1] = $ffi->new("int[1]", false);
$v[1][0] = 42;
var_dump($v);
FFI::free($v[1]);
var_dump($v);
?>
--EXPECTF--
object(FFI\CData:int32_t*[2])#%d (2) {
  [0]=>
  NULL
  [1]=>
  object(FFI\CData:int32_t*)#%d (1) {
    [0]=>
    int(42)
  }
}
object(FFI\CData:int32_t*[2])#%d (2) {
  [0]=>
  NULL
  [1]=>
  NULL
}
