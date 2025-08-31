--TEST--
Argument-less return from by-ref function
--FILE--
<?php

function &test() {
    return;
}

$ref =& test();

?>
--EXPECTF--
Notice: Only variable references should be returned by reference in %s on line %d
