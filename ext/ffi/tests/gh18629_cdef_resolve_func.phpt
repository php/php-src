--TEST--
GH-18629 (FFI::cdef() leaks on function resolution failure)
--EXTENSIONS--
ffi
--INI--
ffi.enable=1
--FILE--
<?php
try {
	$ffi = FFI::cdef("void nonexistent_ffi_test_func(void);");
} catch (\FFI\Exception $e) {
	echo $e->getMessage() . "\n";
}
?>
--EXPECT--
Failed resolving C function 'nonexistent_ffi_test_func'
