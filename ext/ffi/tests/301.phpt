--TEST--
FFI 301: FFI loading
--EXTENSIONS--
ffi
--INI--
ffi.enable=1
--FILE--
<?php
$ffi = FFI::load(__DIR__ . "/300.h");
$ffi->printf("Hello World from %s!\n", "PHP");
?>
--EXPECT--
Hello World from PHP!
