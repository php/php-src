--TEST--
FFI: no memory leak when zend_ffi_finalize_type() bails out inside a declarator
--EXTENSIONS--
ffi
--INI--
ffi.enable=1
--FILE--
<?php

function leaks(string $code): bool {
    $n = 200;
    for ($i = 0; $i < 20; $i++) {
        try { FFI::cdef($code); } catch (FFI\ParserException) {}
    }
    $before = memory_get_usage();
    for ($i = 0; $i < $n; $i++) {
        try { FFI::cdef($code); } catch (FFI\ParserException) {}
    }
    /* A leaked block is at least 16 bytes, so this catches a leak per call */
    return memory_get_usage() - $before >= $n;
}

$cases = [
    // zend_ffi_make_pointer_type()
    'double _Complex *x;',
    'double _Complex **x;',
    // zend_ffi_make_func_type()
    'double _Complex f(int a, int b);',
    // zend_ffi_add_arg(): first and later argument
    'void f(double _Complex x);',
    'void f(int a, double _Complex x);',
    // zend_ffi_nested_declaration()
    'double _Complex (x[3]);',
    'double _Complex (f(int a));',
];

foreach ($cases as $code) {
    try {
        FFI::cdef($code);
        echo "no error: $code\n";
    } catch (FFI\ParserException $e) {
        echo $e->getMessage(), "\n";
    }
    if (leaks($code)) {
        echo "leak: $code\n";
    }
}

?>
--EXPECT--
Unsupported type _Complex at line 1
Unsupported type _Complex at line 1
Unsupported type _Complex at line 1
Unsupported type _Complex at line 1
Unsupported type _Complex at line 1
Unsupported type _Complex at line 1
Unsupported type _Complex at line 1
