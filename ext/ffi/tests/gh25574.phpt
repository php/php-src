--TEST--
GH-25574 (FFI passes bools to variadic functions without promoting them to int)
--EXTENSIONS--
ffi
--SKIPIF--
<?php require_once('utils.inc'); ?>
--INI--
ffi.enable=1
--FILE--
<?php
require_once('utils.inc');
$ffi = ffi_cdef("int ap_php_snprintf(char *s, size_t n, const char *fmt, ...);", ffi_get_php_dll_name());
$buf = $ffi->new("char[64]");
$ffi->ap_php_snprintf($buf, 64, "%d %d %d %d %d %d %d %d", -1, -1, -1, -1, -1, -1, -1, -1);
$ffi->ap_php_snprintf($buf, 64, "%d %d %d %d %d %d %d %d", true, false, true, false, true, false, true, false);
var_dump(FFI::string($buf));
?>
--EXPECT--
string(15) "1 0 1 0 1 0 1 0"
