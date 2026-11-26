--TEST--
GH-25573 (FFI '\a' character constant)
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
var_dump(size("'\\a'"));
?>
--EXPECT--
int(7)
