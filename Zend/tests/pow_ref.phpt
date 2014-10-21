--TEST--
Use power operator on reference
--FILE--
<?php

$a = 2;
$b = 3;

$ref =& $b;

$a **= $b;

var_dump($a);

?>
--EXPECT--
int(8)
