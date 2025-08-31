--TEST--
FFI 016: Structure constraints
--EXTENSIONS--
ffi
--INI--
ffi.enable=1
--FILE--
<?php
try {
    FFI::cdef("struct X {void x();};");
    echo "ok\n";
} catch (Throwable $e) {
    echo get_class($e) . ": " . $e->getMessage()."\n";
}
try {
    FFI::cdef("struct X {struct X x;};");
    echo "ok\n";
} catch (Throwable $e) {
    echo get_class($e) . ": " . $e->getMessage()."\n";
}
try {
    FFI::cdef("struct X {struct X *ptr;};");
    echo "ok\n";
} catch (Throwable $e) {
    echo get_class($e) . ": " . $e->getMessage()."\n";
}
?>
ok
--EXPECT--
FFI\ParserException: function type is not allowed at line 1
FFI\ParserException: Struct/union can't contain an instance of itself at line 1
ok
ok
