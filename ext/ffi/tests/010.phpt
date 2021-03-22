--TEST--
FFI 010: string()
--EXTENSIONS--
ffi
--INI--
ffi.enable=1
--FILE--
<?php
$a = FFI::new("int[3]");
FFI::memset($a, ord("a"), FFI::sizeof($a));
var_dump(FFI::string($a, FFI::sizeof($a)));
?>
--EXPECT--
string(12) "aaaaaaaaaaaa"