--TEST--
GH-7867 (FFI::cast() from pointer to array is broken)
--SKIPIF--
<?php
if (!extension_loaded("ffi")) die("skip ffi extension not available");
?>
--FILE--
<?php
$value = FFI::new('char[26]');
FFI::memcpy($value, implode('', range('a', 'z')), 26);

$slice = FFI::new('char[4]');

echo 'cast from start' . PHP_EOL;
FFI::memcpy($slice, $value, 4);
var_dump($value + 0, $slice, FFI::cast('char[4]', $value));
echo PHP_EOL;

echo 'cast with offset' . PHP_EOL;
FFI::memcpy($slice, $value + 4, 4);
var_dump($value + 4, $slice, FFI::cast('char[4]', $value + 4));
echo PHP_EOL;
?>
--EXPECTF--
cast from start
object(FFI\CData:char*)#%d (1) {
  [0]=>
  string(1) "a"
}
object(FFI\CData:char[4])#%d (4) {
  [0]=>
  string(1) "a"
  [1]=>
  string(1) "b"
  [2]=>
  string(1) "c"
  [3]=>
  string(1) "d"
}
object(FFI\CData:char[4])#%d (4) {
  [0]=>
  string(1) "a"
  [1]=>
  string(1) "b"
  [2]=>
  string(1) "c"
  [3]=>
  string(1) "d"
}

cast with offset
object(FFI\CData:char*)#%d (1) {
  [0]=>
  string(1) "e"
}
object(FFI\CData:char[4])#%d (4) {
  [0]=>
  string(1) "e"
  [1]=>
  string(1) "f"
  [2]=>
  string(1) "g"
  [3]=>
  string(1) "h"
}
object(FFI\CData:char[4])#%d (4) {
  [0]=>
  string(1) "e"
  [1]=>
  string(1) "f"
  [2]=>
  string(1) "g"
  [3]=>
  string(1) "h"
}
