--TEST--
Range info for references (2)
--FILE--
<?php

function test() {
    escape_x($x);
    $x = 0;
    modify_x();
    return PHP_INT_MAX + (int) $x;
}

function escape_x(&$x) {
    $GLOBALS['x'] =& $x;
}

function modify_x() {
    $GLOBALS['x']++;
}

var_dump(test());

?>
--EXPECTF--
float(%s)
