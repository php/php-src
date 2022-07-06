--TEST--
Bug #79749 (Converting FFI instances to bool fails)
--SKIPIF--
<?php
if (!extension_loaded('ffi')) die('skip ffi extension not available');
?>
--FILE--
<?php
$ffi = FFI::cdef('typedef int dummy;');
var_dump((bool) $ffi);
var_dump((bool) FFI::type('int'));
var_dump((bool) FFI::new('int'));
?>
--EXPECT--
bool(true)
bool(true)
bool(true)
