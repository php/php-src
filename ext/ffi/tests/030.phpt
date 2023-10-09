--TEST--
FFI 030: bool type
--EXTENSIONS--
ffi
--INI--
ffi.enable=1
--FILE--
<?php
$ffi = FFI::cdef();

var_dump(FFI::sizeof($ffi->new("bool[2]")));
$p = $ffi->new("bool[2]");
var_dump($p);
$p[1] = true;
var_dump($p[0]);
var_dump($p[1]);
?>
--EXPECTF--
int(2)
object(FFI\CData:bool[2])#%d (2) {
  [0]=>
  bool(false)
  [1]=>
  bool(false)
}
bool(false)
bool(true)
