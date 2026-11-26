--TEST--
GH-25573 (FFI % with a floating operand)
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
var_dump(size("(1.0 % 2) ? 7 : 13"));
var_dump(size("(1 % 2.0) ? 7 : 13"));
var_dump(size("(7 % 4) ? 7 : 13"));
?>
--EXPECT--
string(55) "Invalid operand of '%' in constant expression at line 1"
string(55) "Invalid operand of '%' in constant expression at line 1"
int(7)
