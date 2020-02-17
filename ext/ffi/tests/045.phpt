--TEST--
FFI 045: FFI::isNull()
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--INI--
ffi.enable=1
--FILE--
<?php
var_dump(FFI::isNull(FFI::new("int*")));
$i = FFI::new("int");
var_dump(FFI::isNull(FFI::addr($i)));
try {
	var_dump(FFI::isNull(null));
} catch (Throwable $e) {
	echo get_class($e) . ": " . $e->getMessage()."\n";
}
try {
	var_dump(FFI::isNull(FFI::new("int[0]")));
} catch (Throwable $e) {
	echo get_class($e) . ": " . $e->getMessage()."\n";
}
?>
--EXPECTF--
bool(true)
bool(false)

Warning: FFI::isNull() expects parameter 1 to be FFI\CData, null given in %s045.php on line %d
NULL
FFI\Exception: Cannot instantiate FFI\CData of zero size
