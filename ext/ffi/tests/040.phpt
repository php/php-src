--TEST--
FFI 040: Support for scalar types
--EXTENSIONS--
ffi
--SKIPIF--
<?php
if (pack('S', 0xABCD) !== pack('v', 0xABCD)) {
    die('skip for little-endian architectures only');
}
?>
--INI--
ffi.enable=1
--FILE--
<?php
$x = FFI::new("int");
$x->cdata = 5;
var_dump($x);
var_dump(FFI::typeof($x));
var_dump(FFI::cast("int8_t[4]", $x));
$p = FFI::addr($x);
var_dump($p);
$p[0] += 2;
var_dump($x);
var_dump(FFI::sizeof($x));
var_dump(FFI::alignof($x));
FFI::memset($x, ord("a"), 4);
var_dump(FFI::string($x, 4));

echo "\n";

$y = FFI::new("int[2]");
$y[0] = 6;
var_dump($y[0]);
var_dump(FFI::typeof($y[0]));
var_dump(FFI::cast("int8_t[4]", $y[0]));
$p = FFI::addr($y[0]);
var_dump($p);
$p[0] += 2;
var_dump($y[0]);
var_dump(FFI::sizeof($y[0]));
var_dump(FFI::alignof($y[0]));
FFI::memset($y[0], ord("b"), 4);
var_dump(FFI::string($y[0], 4));

echo "\n";

var_dump(FFI::memcmp($x, $y[0], 4));
FFI::memcpy($x, $y[0], 4);
var_dump(FFI::memcmp($x, $y[0], 4));
?>
--EXPECTF--
object(FFI\CData:int32_t)#%d (1) {
  ["cdata"]=>
  int(5)
}
object(FFI\CType:int32_t)#%d (0) {
}
object(FFI\CData:int8_t[4])#%d (4) {
  [0]=>
  int(5)
  [1]=>
  int(0)
  [2]=>
  int(0)
  [3]=>
  int(0)
}
object(FFI\CData:int32_t*)#%d (1) {
  [0]=>
  int(5)
}
object(FFI\CData:int32_t)#%d (1) {
  ["cdata"]=>
  int(7)
}
int(4)
int(4)
string(4) "aaaa"

int(6)
object(FFI\CType:int32_t)#%d (0) {
}
object(FFI\CData:int8_t[4])#%d (4) {
  [0]=>
  int(6)
  [1]=>
  int(0)
  [2]=>
  int(0)
  [3]=>
  int(0)
}
object(FFI\CData:int32_t*)#%d (1) {
  [0]=>
  int(6)
}
int(8)
int(4)
int(4)
string(4) "bbbb"

int(-1)
int(0)
