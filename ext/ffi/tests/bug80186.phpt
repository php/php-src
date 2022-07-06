--TEST--
Bug #80186 (Segfault when iterating over FFI object)
--SKIPIF--
<?php
if (!extension_loaded('ffi')) die('skip ffi extension not available');
?>
--FILE--
<?php
$ffi = FFI::cdef('typedef int dummy;');
foreach ($ffi as $_) { }
foreach ($ffi as &$_) { }
?>
===DONE===
--EXPECT--
===DONE===
