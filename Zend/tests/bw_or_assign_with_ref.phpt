--TEST--
Bitwise or assign with referenced value
--FILE--
<?php

$num1 = 1;
$num2 = '2';
$ref =& $num2;
$num1 |= $num2;
var_dump($num1);

?>
--EXPECT--
int(3)
