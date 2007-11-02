--TEST--
Bug #31720 (Invalid object callbacks not caught in array_walk())
--FILE--
<?php
$array = array('at least one element');

array_walk($array, array($nonesuchvar,'show'));
?>
===DONE===
--EXPECTF--
Notice: Undefined variable: nonesuchvar in %s on line %d

Warning: array_walk() expects parameter 2 to be valid callback, array given in %s on line %d
===DONE===
