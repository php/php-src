--TEST--
FFI 042: Next array element
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--INI--
ffi.enable=1
--FILE--
<?php
$a = FFI::new("uint8_t[8]");
$a[] = 0;
?>
--EXPECTF--
Fatal error: Uncaught FFI\Exception: Cannot add next element to object of type FFI\CData in %s:3
Stack trace:
#0 {main}
  thrown in %s on line 3
