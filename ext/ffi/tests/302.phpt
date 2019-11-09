--TEST--
FFI 302: FFI preloading
--SKIPIF--
<?php require_once('skipif.inc'); ?>
<?php if (substr(PHP_OS, 0, 3) == 'WIN') die('skip not for Windows'); ?>
--INI--
ffi.enable=1
ffi.preload={PWD}/300.h
--FILE--
<?php
$ffi = FFI::scope("TEST_300");
$ffi->printf("Hello World from %s!\n", "PHP");
?>
--EXPECT--
Hello World from PHP!
