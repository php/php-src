--TEST--
FFI 019: Parameter type adjustment
--EXTENSIONS--
ffi
--INI--
ffi.enable=1
--FILE--
<?php
try {
    FFI::cdef("static int foo(int[]);");
    echo "ok\n";
} catch (Throwable $e) {
    echo get_class($e) . ": " . $e->getMessage()."\n";
}
try {
    FFI::cdef("static int foo(int bar(int));");
    echo "ok\n";
} catch (Throwable $e) {
    echo get_class($e) . ": " . $e->getMessage()."\n";
}
?>
ok
--EXPECT--
ok
ok
ok
