--TEST--
FFI 023: GCC struct extensions
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--INI--
ffi.enable=1
--FILE--
<?php
	var_dump(FFI::sizeof(FFI::new("struct {}")));
	var_dump(FFI::sizeof(FFI::new("struct {int a}")));
	var_dump(FFI::sizeof(FFI::new("struct {int a; int b}")));
?>
ok
--EXPECT--
int(0)
int(4)
int(8)
ok
