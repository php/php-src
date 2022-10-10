--TEST--
GH-9697 (array_walk($ffiInstance, function () {}) crashes due to expecting mutable array)
--SKIPIF--
<?php
if (!extension_loaded("ffi")) die("skip ffi extension not available");
?>
--FILE--
<?php
$x = FFI::new('int');
array_walk($x, function($x) { echo "test\n"; });
?>
DONE
--EXPECT--
DONE
