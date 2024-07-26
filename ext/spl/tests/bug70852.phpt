--TEST--
Bug #70852 Segfault getting NULL offset of an ArrayObject
--FILE--
<?php
$y = new ArrayObject();

var_dump($y[NULL]);
$ref =& $y[NULL];
var_dump($ref);
?>
--EXPECTF--
Warning: Undefined array key "" in %s on line %d
NULL
NULL
