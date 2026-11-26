--TEST--
GH-25573 (FFI sizeof and __alignof__ of a character constant)
--EXTENSIONS--
ffi
--INI--
ffi.enable=1
--FILE--
<?php
function size(string $expr) {
    try {
        $ffi = FFI::cdef("typedef char A[$expr];");
        return FFI::sizeof($ffi->type("A"));
    } catch (FFI\ParserException $e) {
        return $e->getMessage();
    }
}
var_dump(size("sizeof('a')"));
var_dump(size("__alignof__('a')"));
?>
--EXPECT--
int(4)
int(4)
