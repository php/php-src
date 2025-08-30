--TEST--
Bug #70852 Segfault getting NULL offset of an ArrayObject
--FILE--
<?php
$y = new ArrayObject();

var_dump($y[NULL]);
var_dump($y[NULL]++);
?>
--EXPECTF--
Deprecated: Using null as an array offset is deprecated, use an empty string instead in %s on line %d

Warning: Undefined array key "" in %s on line %d
NULL

Deprecated: Using null as an array offset is deprecated, use an empty string instead in %s on line %d

Warning: Undefined array key "" in %s on line %d
NULL
