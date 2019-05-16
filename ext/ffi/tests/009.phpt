--TEST--
FFI 009: memcpy(), memcmp(), memset() and sizeof()
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--INI--
ffi.enable=1
--FILE--
<?php
$a = FFI::new("int[3]");
$a[1] = 10;
$a[2] = 20;
$b = FFI::new("int[4]");
var_dump(FFI::memcmp($b, $a, FFI::sizeof($a)));
FFI::memcpy($b, $a, FFI::sizeof($a));
var_dump($b);
var_dump(FFI::memcmp($b, $a, FFI::sizeof($a)));
FFI::memset($a, -1, FFI::sizeof($a));
var_dump($a);
?>
--EXPECTF--
int(-1)
object(FFI\CData:int32_t[4])#%d (4) {
  [0]=>
  int(0)
  [1]=>
  int(10)
  [2]=>
  int(20)
  [3]=>
  int(0)
}
int(0)
object(FFI\CData:int32_t[3])#%d (3) {
  [0]=>
  int(-1)
  [1]=>
  int(-1)
  [2]=>
  int(-1)
}
