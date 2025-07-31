--TEST--
GH-12102: Incorrect "Cannot use temporary expression in write context" error for BP_VAR_FUNC_ARG
--FILE--
<?php

function test() {
    global $ref;
    byVal(getRef()[0]);
    var_dump($ref);
    byRef(getRef()[0]);
    var_dump($ref);
}

/* Intentionally declared after test() to avoid compile-time checking of ref args. */

function &getRef() {
    global $ref;
    $ref = [];
    return $ref;
}

function byVal($arg) {
    $arg[] = 42;
}

function byRef(&$arg) {
    $arg[] = 42;
}

test();

?>
--EXPECTF--
Warning: Undefined array key 0 in %s on line %d
array(0) {
}
array(1) {
  [0]=>
  array(1) {
    [0]=>
    int(42)
  }
}
