--TEST--
Inline cache can not be used for ZEND_ACC_NEVER_CACHE functions
--FILE--
<?php

$fns = [
    'return function ($x) { var_dump("A"); };',
    'return function ($x) { var_dump("B"); };',
];

function f($f) {
    $f(0);
}

foreach ($fns as $f) {
    f(eval($f));
}

foreach ($fns as $f) {
    $f = eval($f);
    // Closure::__invoke() is ZEND_ACC_NEVER_CACHE
    f($f->__invoke(?));
}

?>
--EXPECT--
string(1) "A"
string(1) "B"
string(1) "A"
string(1) "B"
