--TEST--
GH-12102: Incorrect "Cannot use temporary expression in write context" error for BP_VAR_FUNC_ARG
--FILE--
<?php

function test() {
    byVal(func_get_args()[0]);
    try {
        byRef(func_get_args()[0]);
    } catch (Error $e) {
        echo $e->getMessage(), "\n";
    }
}

/* Intentionally declared after test() to avoid compile-time checking of ref args. */

function byVal($arg) {
    var_dump($arg);
}

function byRef(&$arg) {
    var_dump($arg);
}

test('y');

?>
--EXPECT--
string(1) "y"
Cannot use temporary expression in write context
