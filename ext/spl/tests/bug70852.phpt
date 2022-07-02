--TEST--
Bug #70852 Segfault getting NULL offset of an ArrayObject
--FILE--
<?php
$y = new ArrayObject();

var_dump($y[NULL]);
var_dump($y[NULL]++);
?>
--EXPECTF--
Warning: Undefined array key "" in %s on line %d
NULL

Warning: Undefined array key "" in %s on line %d
NULL
