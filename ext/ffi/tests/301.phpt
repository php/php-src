--TEST--
FFI 301: FFI loading
--SKIPIF--
<?php require_once('skipif.inc'); ?>
<?php if (substr(PHP_OS, 0, 3) == 'WIN') die('skip not for Windows'); ?>
--INI--
ffi.enable=1
--FILE--
<?php
$ffi = FFI::load(__DIR__ . "/300.h");
$ffi->printf("Hello World from %s!\n", "PHP");
?>
--EXPECT--
Hello World from PHP!
