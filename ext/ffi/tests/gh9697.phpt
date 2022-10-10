--TEST--
GH-9697 (array_walk($ffiInstance, function () {}) crashes due to expecting mutable array)
--EXTENSIONS--
ffi
--INI--
ffi.enable=1
--FILE--
<?php
$x = FFI::new('int');
array_walk($x, function($x) { echo "test\n"; });
?>
DONE
--EXPECT--
DONE
