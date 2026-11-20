--TEST--
GH-25573 (FFI constant expression evaluation)
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
var_dump(size("'\\xA'"));
var_dump(size("'\\x41'"));
var_dump(size("'\\x1f'"));
var_dump(size("sizeof('a')"));
var_dump(size("(0xFFFFFFFFu > 'A') ? 7 : 13"));
$ffi = FFI::cdef("typedef _Alignas(16) char T;");
var_dump(FFI::alignof($ffi->type("T")));
$ffi = FFI::cdef("typedef _Alignas(0) int T;");
var_dump(FFI::alignof($ffi->type("T")));
try { FFI::cdef("typedef _Alignas(3) char T;"); } catch (FFI\ParserException $e) { echo $e->getMessage(), "\n"; }
var_dump(size("(1/0) ? 7 : 13"));
var_dump(size("(1%0) ? 7 : 13"));
var_dump(size("((-9223372036854775807 - 1) / -1) ? 7 : 13"));
var_dump(size("(1/2) ? 7 : 13"));
var_dump(size("(1.0 % 0) ? 7 : 13"));
var_dump(size("(1 % 0.0) ? 7 : 13"));
var_dump(size("((-9223372036854775807.0 - 1) % -1) ? 7 : 13"));
var_dump(size("(7 % 4) ? 7 : 13"));
?>
--EXPECT--
int(7)
int(10)
int(65)
int(31)
int(4)
int(7)
int(16)
int(4)
Invalid alignment 3 at line 1
string(61) "Division by zero or overflow in constant expression at line 1"
string(61) "Division by zero or overflow in constant expression at line 1"
string(61) "Division by zero or overflow in constant expression at line 1"
int(13)
string(55) "Invalid operand of '%' in constant expression at line 1"
string(55) "Invalid operand of '%' in constant expression at line 1"
string(55) "Invalid operand of '%' in constant expression at line 1"
int(7)
