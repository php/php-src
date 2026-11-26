--TEST--
GH-25573 (FFI constant expression division by zero or overflow)
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
    "(1/0) ? 7 : 13",
    "(1%0) ? 7 : 13",
    "(1u/0) ? 7 : 13",
    "('a'/0) ? 7 : 13",
    "(1/'\\0') ? 7 : 13",
    "((-9223372036854775807 - 1) / -1) ? 7 : 13",
    "((-9223372036854775807 - 1) % -1) ? 7 : 13",
    "((-9223372036854775807 - 1) / '\\377') ? 7 : 13",
    "((-9223372036854775807 - 1) % '\\377') ? 7 : 13",
    "(1.0 % 0) ? 7 : 13",
    "(1 % 0.0) ? 7 : 13",
    "(1 % 0.5) ? 7 : 13",
    "((-9223372036854775807.0 - 1) % -1) ? 7 : 13",
    // not trapping
    "(1/2) ? 7 : 13",
    "(7 % 4) ? 7 : 13",
    "(1.0/0) ? 7 : 13",
    "((-9223372036854775807 - 1) / 0xffffffffffffffffu) ? 7 : 13",
    "((-9223372036854775807 - 1) % -2) ? 7 : 13",
] as $expr) {
    echo $expr, ": ";
    var_dump(size($expr));
}
?>
--EXPECT--
(1/0) ? 7 : 13: string(61) "Division by zero or overflow in constant expression at line 1"
(1%0) ? 7 : 13: string(61) "Division by zero or overflow in constant expression at line 1"
(1u/0) ? 7 : 13: string(61) "Division by zero or overflow in constant expression at line 1"
('a'/0) ? 7 : 13: string(61) "Division by zero or overflow in constant expression at line 1"
(1/'\0') ? 7 : 13: string(61) "Division by zero or overflow in constant expression at line 1"
((-9223372036854775807 - 1) / -1) ? 7 : 13: string(61) "Division by zero or overflow in constant expression at line 1"
((-9223372036854775807 - 1) % -1) ? 7 : 13: string(61) "Division by zero or overflow in constant expression at line 1"
((-9223372036854775807 - 1) / '\377') ? 7 : 13: string(61) "Division by zero or overflow in constant expression at line 1"
((-9223372036854775807 - 1) % '\377') ? 7 : 13: string(61) "Division by zero or overflow in constant expression at line 1"
(1.0 % 0) ? 7 : 13: string(61) "Division by zero or overflow in constant expression at line 1"
(1 % 0.0) ? 7 : 13: string(61) "Division by zero or overflow in constant expression at line 1"
(1 % 0.5) ? 7 : 13: string(61) "Division by zero or overflow in constant expression at line 1"
((-9223372036854775807.0 - 1) % -1) ? 7 : 13: string(61) "Division by zero or overflow in constant expression at line 1"
(1/2) ? 7 : 13: int(13)
(7 % 4) ? 7 : 13: int(7)
(1.0/0) ? 7 : 13: int(7)
((-9223372036854775807 - 1) / 0xffffffffffffffffu) ? 7 : 13: int(13)
((-9223372036854775807 - 1) % -2) ? 7 : 13: int(13)
