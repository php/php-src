--TEST--
FFI: Test deprecated use of array helper functions on FFI classes doesn't crash
--EXTENSIONS--
ffi
--INI--
ffi.enable=1
--FILE--
<?php
error_reporting(E_ALL & ~E_DEPRECATED);
$data = FFI::cdef()->new('int');
var_dump(reset($data));
var_dump(end($data));
var_dump(next($data));
var_dump(prev($data));
?>
--EXPECTF--
bool(false)
bool(false)
bool(false)
bool(false)
