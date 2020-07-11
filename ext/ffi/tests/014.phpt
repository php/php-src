--TEST--
FFI 014: Size of nested types
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--INI--
ffi.enable=1
--FILE--
<?php
var_dump(FFI::sizeof(FFI::new("uint32_t[2]")));
var_dump(FFI::sizeof(FFI::new("uint32_t([2])")));
var_dump(FFI::sizeof(FFI::new("uint32_t([2])[2]")));
?>
ok
--EXPECT--
int(8)
int(8)
int(16)
ok