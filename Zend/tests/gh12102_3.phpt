--TEST--
GH-12102: Incorrect "Cannot use temporary expression in write context" error for BP_VAR_FUNC_ARG
--FILE--
<?php

function test() {
    byVal(C[0]);
    try {
        byRef(C[0]);
    } catch (Throwable $e) {
        echo $e::class, ': ', $e->getMessage(), "\n";
    }
}

/* Intentionally declared after test() to avoid compile-time checking of ref args. */

const C = ['foo'];

function byVal($arg) {
    var_dump($arg);
}

function byRef(&$arg) {
    var_dump($arg);
}

test('y');

?>
--EXPECT--
string(3) "foo"
Error: Cannot use temporary expression in write context
