--TEST--
Bug #78543 (is_callable() on FFI\CData throws Exception)
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
$ffi = FFI::cdef(' struct test { int dummy; }; ');
$test = $ffi->new('struct test');
var_dump(is_callable($test));
?>
--EXPECT--
bool(false)
