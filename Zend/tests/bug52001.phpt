--TEST--
Bug #52001 (Memory allocation problems after using variable variables)
--FILE--
<?php
a(0,$$var);

$temp1=1;
$temp2=2;
var_dump($temp1);

function a($b,$c) {}
?>
--EXPECTF--
Warning: Undefined variable $var in %s on line %d

Warning: Undefined variable $ in %s on line %d
int(1)
