--TEST--
GH-25573 (FFI _Alignas with an integer constant)
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
$ffi = FFI::cdef("typedef _Alignas(16) char T;");
var_dump(FFI::alignof($ffi->type("T")));
$ffi = FFI::cdef("typedef _Alignas(0) int T;");
var_dump(FFI::alignof($ffi->type("T")));
try {
    FFI::cdef("typedef _Alignas(3) char T;");
} catch (FFI\ParserException $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECT--
int(16)
int(4)
Invalid alignment 3 at line 1
