--TEST--
Bug #78543 (is_callable() on FFI\CData throws Exception)
--EXTENSIONS--
ffi
--FILE--
<?php
$ffi = FFI::cdef(' struct test { int dummy; }; ');
$test = FFI::new('struct test', cdef : $ffi);
var_dump(is_callable($test));
?>
--EXPECT--
bool(false)
