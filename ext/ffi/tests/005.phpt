--TEST--
FFI 005: Array assignment
--EXTENSIONS--
ffi
--INI--
ffi.enable=1
--FILE--
<?php
$ffi = FFI::cdef();

$m = $ffi->new("int[2][2]");
$v = $ffi->new("int[2]");
$v[1] = 42;
$m[1] = $v;
var_dump($m);
?>
--EXPECTF--
object(FFI\CData:int32_t[2][2])#%d (2) {
  [0]=>
  object(FFI\CData:int32_t[2])#%d (2) {
    [0]=>
    int(0)
    [1]=>
    int(0)
  }
  [1]=>
  object(FFI\CData:int32_t[2])#%d (2) {
    [0]=>
    int(0)
    [1]=>
    int(42)
  }
}
