--TEST--
GH-25574 (FFI passes bools to variadic functions without promoting them to int)
--EXTENSIONS--
ffi
--INI--
ffi.enable=1
--FILE--
<?php
$ffi = FFI::cdef("int snprintf(char *s, size_t n, const char *fmt, ...);");
$buf = $ffi->new("char[64]");
$ffi->snprintf($buf, 64, "%d %d %d %d %d %d %d %d", -1, -1, -1, -1, -1, -1, -1, -1);
$ffi->snprintf($buf, 64, "%d %d %d %d %d %d %d %d", true, false, true, false, true, false, true, false);
var_dump(FFI::string($buf));
?>
--EXPECT--
string(15) "1 0 1 0 1 0 1 0"
