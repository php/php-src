--TEST--
GH-9697 (array_walk($ffiInstance, function () {}) crashes due to expecting mutable array)
--EXTENSIONS--
ffi
--INI--
ffi.enable=1
--FILE--
<?php
$x = FFI::cdef()->new('int');
array_walk($x, function($x) { echo "test\n"; });
?>
DONE
--EXPECTF--
Deprecated: array_walk(): Passing an object for argument #1 $array to array_walk() is deprecated, call get_object_vars() first instead in %s on line %d
DONE
