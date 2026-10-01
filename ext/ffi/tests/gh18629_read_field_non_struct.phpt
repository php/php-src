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
} catch (\Throwable $e) {
	echo $e::class, ': ', $e->getMessage(), "\n";
}
?>
--EXPECT--
FFI\Exception: Attempt to read field 'foo' of non C struct/union
