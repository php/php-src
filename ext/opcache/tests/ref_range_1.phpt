--TEST--
Range info for references (1)
--FILE--
<?php

function test() {
    escape_x($x);
    $x = 0;
    modify_x();
    return (int) $x;
}

function escape_x(&$x) {
    $GLOBALS['x'] =& $x;
}

function modify_x() {
    $GLOBALS['x']++;
}

var_dump(test());

?>
--EXPECT--
int(1)
