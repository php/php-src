--TEST--
FFI 027: Incomplete and variable length arrays
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--INI--
ffi.enable=1
--FILE--
<?php
try {
    $p = FFI::new("int[*]");
    echo "ok\n";
} catch (Throwable $e) {
    echo get_class($e) . ": " . $e->getMessage()."\n";
}
try {
    FFI::cdef("static int (*foo)[*];");
    echo "ok\n";
} catch (Throwable $e) {
    echo get_class($e) . ": " . $e->getMessage()."\n";
}
try {
    FFI::cdef("typedef int foo[*];");
    echo "ok\n";
} catch (Throwable $e) {
    echo get_class($e) . ": " . $e->getMessage()."\n";
}
try {
    FFI::cdef("static void foo(int[*][*]);");
    echo "ok\n";
} catch (Throwable $e) {
    echo get_class($e) . ": " . $e->getMessage()."\n";
}
try {
    var_dump(FFI::sizeof(FFI::new("int[0]")));
} catch (Throwable $e) {
    echo get_class($e) . ": " . $e->getMessage()."\n";
}
try {
    var_dump(FFI::sizeof(FFI::new("int[]")));
} catch (Throwable $e) {
    echo get_class($e) . ": " . $e->getMessage()."\n";
}
try {
    var_dump(FFI::sizeof(FFI::cast("int[]", FFI::new("int[2]"))));
} catch (Throwable $e) {
    echo get_class($e) . ": " . $e->getMessage()."\n";
}
try {
    FFI::cdef("struct _x {int a; int b[];};");
    echo "ok\n";
} catch (Throwable $e) {
    echo get_class($e) . ": " . $e->getMessage()."\n";
}
try {
    $f = FFI::cdef("typedef int(*foo)[];");
    echo "ok\n";
} catch (Throwable $e) {
    echo get_class($e) . ": " . $e->getMessage()."\n";
}
try {
    $f = FFI::cdef("typedef int foo[][2];");
    echo "ok\n";
} catch (Throwable $e) {
    echo get_class($e) . ": " . $e->getMessage()."\n";
}
try {
    $f = FFI::cdef("typedef int foo[];");
    echo "ok\n";
} catch (Throwable $e) {
    echo get_class($e) . ": " . $e->getMessage()."\n";
}
try {
    $f = FFI::cdef("static int foo(int[]);");
    echo "ok\n";
} catch (Throwable $e) {
    echo get_class($e) . ": " . $e->getMessage()."\n";
}
?>
--EXPECT--
FFI\ParserException: "[*]" is not allowed in other than function prototype scope at line 1
FFI\ParserException: "[*]" is not allowed in other than function prototype scope at line 1
FFI\ParserException: "[*]" is not allowed in other than function prototype scope at line 1
ok
FFI\Exception: Cannot instantiate FFI\CData of zero size
FFI\ParserException: "[]" is not allowed at line 1
FFI\ParserException: "[]" is not allowed at line 1
ok
ok
ok
ok
ok
