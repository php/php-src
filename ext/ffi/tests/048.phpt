--TEST--
FFI 048: CData::addrValue() get the value of the pointer variable
--EXTENSIONS--
ffi
--INI--
ffi.enable=1
--FILE--
<?php
$x = FFI::cdef()->cast("void*", 20);
$v = FFI::addrValue($x);
var_dump($v);

$x2 = FFI::cdef()->cast('int*', 30);
$v2 = FFI::addrValue($x2);
var_dump($v2);
?>
--EXPECTF--
int(20)
int(30)
