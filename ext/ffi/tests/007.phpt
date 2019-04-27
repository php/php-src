--TEST--
FFI 007: Pointer comparison
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--INI--
ffi.enable=1
--FILE--
<?php
$v = FFI::new("int*[3]");
$v[0] = FFI::new("int[1]", false);
$v[1] = FFI::new("int[1]", false);
$v[2] = $v[1];
$v[1][0] = 42;
var_dump($v[0] == $v[1]);
var_dump($v[1] == $v[2]);
FFI::free($v[0]);
FFI::free($v[1]);
?>
--EXPECT--
bool(false)
bool(true)
