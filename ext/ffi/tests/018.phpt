--TEST--
FFI 018: Indirectly recursive structure
--EXTENSIONS--
ffi
--INI--
ffi.enable=1
--FILE--
<?php
try {
    FFI::cdef("struct X {struct X x[2];};");
    echo "ok\n";
} catch (Throwable $e) {
    echo get_class($e) . ": " . $e->getMessage()."\n";
}
try {
    FFI::cdef("struct X {struct X *ptr[2];};");
    echo "ok\n";
} catch (Throwable $e) {
    echo get_class($e) . ": " . $e->getMessage()."\n";
}
?>
ok
--EXPECT--
FFI\ParserException: Incomplete struct "X" at line 1
ok
ok
