--TEST--
FFI 013: Declaration priorities and constrains
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--INI--
ffi.enable=1
--FILE--
<?php
$a = FFI::new("int[1][2][3]");
var_dump(count($a));
var_dump(count($a[0]));
var_dump(count($a[0][0]));

try {
    var_dump(FFI::new("void"));
} catch (Throwable $e) {
    echo get_class($e) . ": " . $e->getMessage()."\n";
}

try {
    var_dump(FFI::new("void[1]"));
} catch (Throwable $e) {
    echo get_class($e) . ": " . $e->getMessage()."\n";
}
try {
    FFI::cdef("static int foo(int)[5];");
    echo "ok\n";
} catch (Throwable $e) {
    echo get_class($e) . ": " . $e->getMessage()."\n";
}
try {
    FFI::cdef("static int foo[5](int);");
    echo "ok\n";
} catch (Throwable $e) {
    echo get_class($e) . ": " . $e->getMessage()."\n";
}
try {
    FFI::cdef("static int foo(int)(int);");
    echo "ok\n";
} catch (Throwable $e) {
    echo get_class($e) . ": " . $e->getMessage()."\n";
}
try {
    FFI::cdef("typedef int foo[2][];");
    echo "ok\n";
} catch (Throwable $e) {
    echo get_class($e) . ": " . $e->getMessage()."\n";
}
try {
    FFI::cdef("typedef int foo[][2];");
    echo "ok\n";
} catch (Throwable $e) {
    echo get_class($e) . ": " . $e->getMessage()."\n";
}
?>
--EXPECT--
int(1)
int(2)
int(3)
FFI\ParserException: void type is not allowed at line 1
FFI\ParserException: void type is not allowed at line 1
FFI\ParserException: Function returning array is not allowed at line 1
FFI\ParserException: Array of functions is not allowed at line 1
FFI\ParserException: Function returning function is not allowed at line 1
FFI\ParserException: Only the leftmost array can be undimensioned at line 1
ok
