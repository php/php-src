--TEST--
By-ref assign of WeakMap dimension
--FILE--
<?php
$obj = new stdClass;
$map = new WeakMap;
$int = 0;
$map[$obj] =& $int;
$int++;
var_dump($map[$obj]);
?>
--EXPECT--
int(1)
