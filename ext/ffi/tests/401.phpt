--TEST--
FFI 401: Variabic callback
--SKIPIF--
<?php
require_once('skipif.inc');
?>
<?php
try {
	FFI::cdef("",__DIR__ . '/variabic.so');
} catch (Throwable $e) {
	die('skip variabic.so not load');
}
?>
--INI--
ffi.enable=1
--FILE--
<?php
$cdef = '
typedef int (*variadic_callback)(const char *format,...);
extern int variadic_test(variadic_callback p_callback, char *str,...);
';

function test_printf($f, ...$s)
{
    $sa  = [];
    foreach ($s as $p) {
       $sa[] = FFI::string(FFI::cast('char*', $p));
    }
    printf($f,...$sa);
}

$ffi = FFI::cdef($cdef, __DIR__ . '/variabic.so');
$ffi->variadic_test('test_printf', "variabic test: %s", "input string");

?>
--EXPECT--
variabic test: input string
