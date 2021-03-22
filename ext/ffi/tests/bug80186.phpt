--TEST--
Bug #80186 (Segfault when iterating over FFI object)
--EXTENSIONS--
ffi
--FILE--
<?php
$ffi = FFI::cdef('typedef int dummy;');
foreach ($ffi as $_) { }
foreach ($ffi as &$_) { }
?>
===DONE===
--EXPECT--
===DONE===
