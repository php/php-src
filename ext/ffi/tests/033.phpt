--TEST--
FFI 033: FFI::new(), FFI::free(), FFI::type(), FFI::typeof(), FFI::arrayType()
--EXTENSIONS--
ffi
--INI--
ffi.enable=1
--FILE--
<?php
$p1 = FFI::new("uint8_t[2]");
$p2 = FFI::new("uint16_t[2]", true, true);
var_dump($p1, $p2);

$t1 = FFI::typeof($p1);
var_dump($t1);

$p4 = FFI::new($t1);
var_dump($p4);

$t2 = FFI::type("uint16_t[2]");
var_dump($t2);
$p4 = FFI::new($t2);
var_dump($p4);

$t2 = FFI::type("uint32_t");
var_dump($t2);
$t3 = FFI::arrayType($t2, [2, 2]);
var_dump($t3);
?>
--EXPECTF--
object(FFI\CData:uint8_t[2])#%d (2) {
  [0]=>
  int(0)
  [1]=>
  int(0)
}
object(FFI\CData:uint16_t[2])#%d (2) {
  [0]=>
  int(0)
  [1]=>
  int(0)
}
object(FFI\CType:uint8_t[2])#%d (0) {
}
object(FFI\CData:uint8_t[2])#%d (2) {
  [0]=>
  int(0)
  [1]=>
  int(0)
}
object(FFI\CType:uint16_t[2])#%d (0) {
}
object(FFI\CData:uint16_t[2])#%d (2) {
  [0]=>
  int(0)
  [1]=>
  int(0)
}
object(FFI\CType:uint32_t)#%d (0) {
}
object(FFI\CType:uint32_t[2][2])#%d (0) {
}
