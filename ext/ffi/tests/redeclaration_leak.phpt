--TEST--
FFI: no memory leak on redeclaration errors
--EXTENSIONS--
ffi
--INI--
ffi.enable=1
--FILE--
<?php

$cases = [
    'int x; int *x;',
    'int x; struct { int a; } *x;',
    'typedef struct { int a; } T; typedef struct { int a; } T;',
    'typedef struct { int a; } T, U; typedef struct { int a; } *U;',
    'enum { A }; enum { A };',
    'int A; enum { A };',
    'void f(enum { B, B } e);',
];

foreach ($cases as $code) {
    try {
        FFI::cdef($code);
    } catch (FFI\ParserException $e) {
        echo $e::class, ': ', $e->getMessage(), PHP_EOL;
    }
}

?>
--EXPECT--
FFI\ParserException: Redeclaration of "x" at line 1
FFI\ParserException: Redeclaration of "x" at line 1
FFI\ParserException: Redeclaration of "T" at line 1
FFI\ParserException: Redeclaration of "U" at line 1
FFI\ParserException: Redeclaration of "A" at line 1
FFI\ParserException: Redeclaration of "A" at line 1
FFI\ParserException: Redeclaration of "B" at line 1
