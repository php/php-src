--TEST--
GH-25573 (FFI hex escapes with digits A-F)
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
foreach ([
    "'\\x9'",
    "'\\xA'",
    "'\\xf'",
    "'\\x41'",
    "'\\x1f'",
    "'\\x7F'",
    "'\\x6B'",
    "'\\x5c'",
    "((-9223372036854775807 - 1) / '\\xff') ? 7 : 13",
] as $expr) {
    echo $expr, ": ";
    var_dump(size($expr));
}
?>
--EXPECT--
'\x9': int(9)
'\xA': int(10)
'\xf': int(15)
'\x41': int(65)
'\x1f': int(31)
'\x7F': int(127)
'\x6B': int(107)
'\x5c': int(92)
((-9223372036854775807 - 1) / '\xff') ? 7 : 13: string(61) "Division by zero or overflow in constant expression at line 1"
