--TEST--
FFI 001: Check if FFI is loaded
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--INI--
ffi.enable=1
--FILE--
<?php
echo 'The extension "FFI" is available';
?>
--EXPECT--
The extension "FFI" is available
