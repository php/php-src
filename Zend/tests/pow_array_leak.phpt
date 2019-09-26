--TEST--
Memory leak on ** with result==op1 array
--FILE--
<?php

$x = [0];
$x **= 1;
var_dump($x);

$x = [0];
$x **= $x;
var_dump($x);

?>
--EXPECT--
int(0)
int(0)
