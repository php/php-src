--TEST--
First class callables and references
--FILE--
<?php

function &id(&$x) {
    return $x;
}

$fn = id(...);
$i = 0;
$i2 =& $fn($i);
$i++;
var_dump($i2);

?>
--EXPECT--
int(1)
