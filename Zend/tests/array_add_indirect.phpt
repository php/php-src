--TEST--
Array addition should not add INDIRECT elements
--FILE--
<?php

$x = 1;
$ary = ['y' => 1];
$ary += $GLOBALS;
var_dump($ary['x']);
$x = 2;
var_dump($ary['x']);

?>
--EXPECT--
int(1)
int(1)
