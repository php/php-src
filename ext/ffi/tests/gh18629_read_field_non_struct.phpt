--TEST--
GH-18629 (Read field of non C struct/union)
--EXTENSIONS--
ffi
--INI--
ffi.enable=1
--FILE--
<?php
$x = FFI::cdef()->new("int*");
try {
	$y = $x->foo;
} catch (\FFI\Exception $e) {
	echo $e->getMessage() . "\n";
}
?>
--EXPECT--
Attempt to read field 'foo' of non C struct/union
