--TEST--
GH-12102: Incorrect "Cannot use temporary expression in write context" error for BP_VAR_FUNC_ARG
--FILE--
<?php

function test() {
    byVal(C[0]);
    byRef(C[0]);
    var_dump(C);
}

/* Intentionally declared after test() to avoid compile-time checking of ref args. */

const C = ['foo'];

function byVal($arg) {
    var_dump($arg);
}

function byRef(&$arg) {
    $arg = 'modified';
    var_dump($arg);
}

test('y');

?>
--EXPECT--
string(3) "foo"
string(8) "modified"
array(1) {
  [0]=>
  string(3) "foo"
}
